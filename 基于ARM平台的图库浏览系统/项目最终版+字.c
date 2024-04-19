
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "fontl.c"

#define LCD_PATH         "/dev/fb0"
#define TOUCH_PATH       "/dev/input/event0"
#define PHOTO_PATH       "/photos"
#define PHOTO_PATH_LEN   256
#define PAGE_SIZE        9
#define LCD_W            800
#define LCD_H            480
#define bmp_path_len     50 


typedef struct photo_link_list            
{
char photo_path[PHOTO_PATH_LEN];       
int photo_num;                        
int obj_photo_x;                   
int obj_photo_y;

struct photo_link_list * next;
struct photo_link_list * prev;

}photo_node,*photo_link;


typedef struct electronic_photo_album_inf      
{
    DIR * dp;                                  
    int photo_sum;                              
    int page_sum;                              
    photo_node photo_list_head;                 

}photo_show_node,*photo_show_poin;


int Show_Desktop(const char *bmp_path1,const char *bmp_path2);                 
       
int Show_Bmp(const char *bmp_path);                                     
int Show_Bmp_LEFT(const char*bmp_path);                             
int Show_Bmp_RIGHT(const char*bmp_path);                             
int Show_Bmp_Upper_Right(const char*bmp_path);                        
int Display_Photo(const char * bmp_file_path,int zoomout,int where_x,int where_y);  
int Touch_ret1();                                                                
int Funiction_Select();                                                 
int Touch_LCD();                                                        
int Electronic_photo_Album_Function();                                   

photo_show_poin Electronic_Photo_Album_Init();                                 
int Serach_Photo(photo_show_poin psp);                             
photo_link Create_Photo_List_Node();                                                   
int Count_Page_Photo_Sum(photo_show_poin psp);                     
int Tail_Add_Photo_Node(photo_show_poin psp,photo_link new_node);       

int Thumbnail_Ui(int page_num,photo_show_poin psp);                    
int Touch_ret();                                                      
int Distory_Photo_List(photo_show_poin psp);                         

int Show_character();                                                
int show_charact();

int page_n=1;

int Touch_ret1()                                                     
{

    int ts=open(TOUCH_PATH,O_RDONLY);
    if(ts == -1)
    {
        perror("open failed");
        return -1;
    }

    struct input_event touch; 
    int x=0,y=0;

    while(1)
    {
         
         if(read(ts,&touch,sizeof(touch)) ==-1)              
        {
            perror("read failed");
            return -1;
        }

        if(touch.type==EV_ABS && touch.code ==ABS_X)  
        x=touch.value;   

        if(touch.type==EV_ABS && touch.code ==ABS_Y)   
        y=touch.value;  
        if((x>0 && x<400)&&(y>0 && y<480))    
        {   
            close(ts);
            return 1;
        }
        else if((x>400 && x<700)&&(y>100 && y<480))  
        { 
            close(ts);
            return 2;
        }
        else if((x>700 && x<800)&&(y>0 && y<100))
        {
            close(ts);
            return 3;
        }
    }
    return 0;
}


int Show_Desktop(const char *bmp_path1,const char *bmp_path2)    
{

    if(Show_Bmp(bmp_path1)==-1)                                
    {
        printf("显示图片失败！");
        return -1;
    }

    if(Show_Bmp(bmp_path2)==-1)                                
    {
        printf("显示图片失败！");
        return -1;
    }  
     if(Show_Bmp_Upper_Right("/guanji.bmp")==-1)   
    {
        printf("显示返回图标失败！!\n");
        return -1;
    }  

    while(1)
    {
        int ret=0;
        ret=Touch_ret1();
        if(ret==3)
        {
            if(Show_Bmp("/guanji2.bmp")==-1)                                
            {
                printf("显示图片失败！");
                return -1;
            }
            sleep(3);
            if(Show_Bmp("/black.bmp")==-1)                                
            {
                printf("显示图片失败！");
                return -1;
            }
        }

        else if(ret==1)
        {
                            int r=0;
                            r=Touch_ret1();
                                if(r==2)
                                {
                                    printf("解锁成功，进入功能选择界面！");
                                    if(Funiction_Select()==-1)                     
                                    {
                                        printf("进入功能选择界面失败！");
                                        return -1;
                                    }
                                }
        }

    }

return 0;

}


int Show_Bmp(const char *bmp_path)                 
{
    int lcd_fd = open(LCD_PATH,O_RDWR);    
    if(lcd_fd == -1)
    {
        perror("open ... ");
        return -1;
    }

    int bmp_fd = open(bmp_path,O_RDONLY);       
    if(bmp_fd == -1)
    {
        perror("open ... ");
        return -1;
    }
                                                   
    int * mmap_start_p = (int *)mmap(NULL,LCD_W*LCD_H*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);
    if(mmap_start_p == (int *)MAP_FAILED)
    {
        perror("mmap ... ");
        return -1;
    }

    int w=0;    
    int h=0;
        
    lseek(bmp_fd,18,SEEK_SET);   

    if(read(bmp_fd,&w,sizeof(int))==-1)    
    {
        perror("read failed");
        return -1;
    }

    if(read(bmp_fd,&h,sizeof(int))==-1)    
    {
        perror("read failed");
        return -1;
    }

    char rgb[w*h*3];    

    lseek(bmp_fd,54,SEEK_SET);

    if(read(bmp_fd,rgb,sizeof(rgb))==-1)    
    {
        perror("read failed");
        return -1;
    }

    for(int y=0,n=0;y<h;y++)             
    {
        for(int x=0;x<w;x++,n+=3)
        {
            *(mmap_start_p+LCD_W*(h-1-y)+x)=rgb[n]<<0 | rgb[n+1]<<8 | rgb[n+2]<<16;
        }

    }

    close(lcd_fd);
    close(bmp_fd);
    munmap(mmap_start_p,LCD_W*LCD_H * 4);
    return 0;

}


int Show_Bmp_LEFT(const char*bmp_path)             
{
    int lcd_fd=open(LCD_PATH,O_RDWR);   
    if(lcd_fd==-1)
    {
        perror("open failed");
        return -1;
    }

    int bmp_fd=open(bmp_path,O_RDONLY);  
    if(bmp_fd==-1)
    {
        perror("open failed");
        return -1;
    }
    int w=0;    
    int h=0;    
   

    lseek(bmp_fd,18,SEEK_SET);  

    if(read(bmp_fd,&w,sizeof(int))==-1)    
    {
        perror("read failed");
        return -1;
    }

    if(read(bmp_fd,&h,sizeof(int))==-1)
    {
        perror("read failed");
        return -1;
    }

    int *mmap_start_p=(int *)mmap(NULL,LCD_H*LCD_W*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);   
    if(mmap_start_p==(int *)MAP_FAILED)
    {
        perror("mmap failed");
        return -1;
    }

    int *new_p = mmap_start_p + (LCD_W * (LCD_H/2 - h/2) + (LCD_W/2 - w/2) -200);
  
    char rgb[w*h*3];    
   
    lseek(bmp_fd,54,SEEK_SET);

    if(read(bmp_fd,rgb,sizeof(rgb))==-1)   
    {
        perror("read failed");
        return -1;
    }

    for(int y=0,n=0;y<h;y++)           
    {
        for(int x=0;x<w;x++,n+=3)
        {
            *(new_p+LCD_W*(h-1-y)+x)=rgb[n]<<0 | rgb[n+1]<<8 | rgb[n+2]<<16;
        }

    }

    if(close(lcd_fd)==-1)
    {
        perror("close failed");
        return -1;
    }

    if(close(bmp_fd)==-1)
    {
        perror("close failed");
        return -1;
    }

    munmap(mmap_start_p,LCD_H*LCD_W*4);

    return 0;
}


int Show_Bmp_RIGHT(const char*bmp_path)           
{

    int lcd_fd=open(LCD_PATH,O_RDWR);  
    if(lcd_fd==-1)
    {
        perror("open failed");
        return -1;
    }

    int bmp_fd=open(bmp_path,O_RDONLY);   
    if(bmp_fd==-1)
    {
        perror("open failed");
        return -1;
    }

    int w=0;    
    int h=0;    
   
    lseek(bmp_fd,18,SEEK_SET);  

    if(read(bmp_fd,&w,sizeof(int))==-1)    
    {
        perror("read failed");
        return -1;
    }

    if(read(bmp_fd,&h,sizeof(int))==-1)   
    {
        perror("read failed");
        return -1;
    }

    int *mmap_start_p=(int *)mmap(NULL,LCD_H*LCD_W*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0); 
    if(mmap_start_p==(int *)MAP_FAILED)
    {
        perror("mmap failed");
        return -1;
    }


    int *new_p = mmap_start_p + (LCD_W * (LCD_H/2 - h/2) + (LCD_W/2 - w/2) +200);  

    char rgb[w*h*3];    
   
    lseek(bmp_fd,54,SEEK_SET);

    if(read(bmp_fd,rgb,sizeof(rgb))==-1)   
    {
        perror("read failed");
        return -1;
    }

    for(int y=0,n=0;y<h;y++)           
    {
        for(int x=0;x<w;x++,n+=3)
        {
            *(new_p+LCD_W*(h-1-y)+x)=rgb[n]<<0 | rgb[n+1]<<8 | rgb[n+2]<<16;
        }

    }

    if(close(lcd_fd)==-1)
    {
        perror("close failed");
        return -1;
    }

    if(close(bmp_fd)==-1)
    {
        perror("close failed");
        return -1;
    }

    munmap(mmap_start_p,LCD_H*LCD_W*4);

    return 0;
}


int Show_Bmp_Upper_Right(const char*bmp_path)     
{

    int lcd_fd=open(LCD_PATH,O_RDWR);   
    if(lcd_fd==-1)
    {
        perror("open failed");
        return -1;
    }

    int bmp_fd=open(bmp_path,O_RDONLY);   
    if(bmp_fd==-1)
    {
        perror("open failed");
        return -1;
    }

    int w=0;    
    int h=0;   
    lseek(bmp_fd,18,SEEK_SET);   

    if(read(bmp_fd,&w,sizeof(int))==-1)   
    {
        perror("read failed");
        return -1;
    }

    if(read(bmp_fd,&h,sizeof(int))==-1)    
    {
        perror("read failed");
        return -1;
    }


    int *mmap_start_p=(int *)mmap(NULL,LCD_H*LCD_W*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);   
    if(mmap_start_p==(int *)MAP_FAILED)
    {
        perror("mmap failed");
        return -1;
    }

    int *new_p=mmap_start_p + (LCD_W +(800-w));

    char rgb[w*h*3];   
  

    lseek(bmp_fd,54,SEEK_SET);

    if(read(bmp_fd,rgb,sizeof(rgb))==-1)    
    {
        perror("read failed");
        return -1;
    }

    for(int y=0,n=0;y<h;y++)            
    {
        for(int x=0;x<w;x++,n+=3)
        {
            *(new_p+LCD_W*(h-1-y)+x)=rgb[n]<<0 | rgb[n+1]<<8 | rgb[n+2]<<16;
        }

    }

    if(close(lcd_fd)==-1)
    {
        perror("close failed");
        return -1;
    }


    if(close(bmp_fd)==-1)
    {
        perror("close failed");
        return -1;
    }
    munmap(mmap_start_p,LCD_H*LCD_W*4);

    return 0;
}


int Touch_LCD()                              
{
 int ts=open(TOUCH_PATH,O_RDONLY);
    if(ts == -1)
    {
        perror("open failed");
        return -1;
    }

    struct input_event touch; 
    int x,y;
    while(1)
    {
         
         if(read(ts,&touch,sizeof(touch)) ==-1)              
        {
            perror("read failed");
            return -1;
        }

        if(touch.type==EV_ABS && touch.code ==ABS_X)  
        x=touch.value;   

        if(touch.type==EV_ABS && touch.code ==ABS_Y)   
        y=touch.value;  
        
        printf("x:%d-----y:%d\n",x,y);
      
        if((x>700 & x<800)&&(y>0 && y<100))                
        {
            return 0;
        }
         else if((y<315 && y>165) && (x>125 && x<275))    
         {
             return 1;
         }
         else if((y<315 && y>165) && (x<675 && x>525))   
         {
             return 2;
         }
        
    }

return 0;
}


int Funiction_Select()
{

    if(Show_Bmp("/function_bmp1.bmp")==-1)
    {
        printf("显示背景图失败!\n");
        return -1;
    }
    if(Show_Bmp_LEFT("/function.bmp")==-1) 
    {
    printf("显示图库图标失败！");
    return -1;          
    }
    if(Show_Bmp_RIGHT("/steam.bmp")==-1)        
    {
        printf("显示游戏图标失败！");
        return -1;
    }
    if(Show_Bmp_Upper_Right("/out.bmp")==-1)   
    {
        printf("显示返回按钮失败!\n");
        return -1;
    }  


    if(Touch_LCD()==0)
    {
        if(Show_Desktop("/window.bmp","/open.bmp")==-1)
        {
            printf("返回上一步失败！\n");
            return -1;
        }
    }
    else if(Touch_LCD()==1)
    {
        printf("进入电子相册！");
        if(Electronic_photo_Album_Function()==-1)    
        {
            printf("进入电子相册失败！");
            return -1;

        }   
    }
    else if(Touch_LCD()==2)
    {
        if(Show_Bmp("/404.bmp")==-1)
        {
            printf("显示背景图失败!\n");
            return -1;
        }

        if(Show_Bmp_Upper_Right("/out.bmp")==-1)   
        {
            printf("显示返回按钮失败!\n");
            return -1;
        }  

        int out=Touch_LCD();
        if(out==0)
        {
            printf("进入程序选择界面！\n");
            Funiction_Select();
        }




    }

    return 0;
}


int Electronic_photo_Album_Function()                       
{

    photo_show_poin psp = Electronic_Photo_Album_Init();    
    if(psp == (photo_show_poin) -1)
    {
        printf("电子相册初始化失败！\n");
        return -1;
    }

    
    Show_character();



   if(Thumbnail_Ui(1,psp)==-1)                                    
    {
        printf("显示电子相册缩略图失败！\n");
        return -1;
    }
    printf("一共有%d页！\n",psp->page_sum);
    printf("一共有%d张照片！\n",psp->photo_sum);
    
    Show_character();

     while(1)
     {
        int ret=0;
        ret=Touch_ret();
        if(ret==1)
        {
            if(page_n++>(psp->page_sum))page_n=1;
            Thumbnail_Ui(page_n,psp);   
            
            Show_character();
        }
        else if(ret==2)                 
        { 
            Thumbnail_Ui(page_n-1,psp);   
            page_n-=1;
            if(page_n<1)page_n=1;
            Show_character();
        }   
        else if(ret==3)                    
        {
            Distory_Photo_List(psp);
            
            Funiction_Select();
            page_n=1;
             
        }
       
        else if(ret>3&&ret<13)
        {
            photo_link new_node; 
           
            for(new_node = &(psp->photo_list_head);new_node->photo_num!=((page_n-1)*9+(ret-3));new_node=new_node->next);
            Show_Bmp(new_node->photo_path);    
            
            if(Show_Bmp_Upper_Right("/out.bmp")==-1)   
            {
            printf("显示返回图标失败!\n");
            return -1;
            }  

            while(1)
            {
                int ret=0;
                ret=Touch_ret();
                if(ret==1)
                {
                    Show_Bmp(new_node->next->photo_path);  
                    new_node=new_node->next;
                    if(Show_Bmp_Upper_Right("/out.bmp")==-1)    
                    {printf("显示返回图标失败!\n");
                    return -1;
                    }
                }  
                
                else if(ret==2)
                { 
                  Show_Bmp(new_node->prev->photo_path);
                   new_node=new_node->prev;
                    if(Show_Bmp_Upper_Right("/out.bmp")==-1)   
                    {printf("显示返回图标失败!\n");
                    return -1;
                    }  
                }
                else if(ret==3)        
                {
                    page_n=1;
                    Electronic_photo_Album_Function();

                }

            }


        }
    }

    return 0;
}


int Distory_Photo_List(photo_show_poin psp)
{
    if (psp->photo_list_head.next == (photo_link)NULL)
    {
        printf("链表头是空的\n");
        return 0;
    }

    while (psp->photo_list_head.next != &(psp->photo_list_head))
    {
    photo_link tem_node = psp->photo_list_head.next;

    tem_node->next->prev = tem_node->prev;
    tem_node->prev->next = tem_node->next;

    free(tem_node);
    }
    psp->photo_list_head.next = psp->photo_list_head.prev = &(psp->photo_list_head);

    return 0;
}




int Touch_ret()
{
    int ts=open(TOUCH_PATH,O_RDONLY);
    if(ts == -1)
    {
        perror("open failed");
        return -1;
    }

    struct input_event touch; 
    int x=0,y=0;

    while(1)
    {
         
         if(read(ts,&touch,sizeof(touch)) ==-1)              
        {
            perror("read failed");
            return -1;
        }

        if(touch.type==EV_ABS && touch.code ==ABS_X)  
        x=touch.value;   

        if(touch.type==EV_ABS && touch.code ==ABS_Y)   
        y=touch.value;  
        
        printf("x:%d-----y:%d\n",x,y);

        if((x>650 & x<800)&&(y>325 && y<480))    
        {
            return 1;
        }
        else if((x>650 & x<800)&&(y>100 && y<325))  
        { 
            return 2;
        }
        else if((x>700 & x<800)&&(y>0 && y<100))   
        {
            return 3;
        }
        else if((x>0 & x<200)&&(y>0 && y<120))   
        {
            return 4;
        }
        else if((x>220 & x<420)&&(y>0 && y<120))   
        {
            return 5;
        }
        else if((x>440 & x<640)&&(y>0 && y<120))    
        {
            return 6;
        }
        else if((x>0 & x<200)&&(y>140 && y<260))    
        {
            return 7;
        }
        else if((x>220 & x<420)&&(y>140 && y<260))    
        {
            return 8;
        }
        else if((x>440 & x<640)&&(y>140 && y<260))    
        {
            return 9;
        }
        else if((x>0 & x<200)&&(y>280 && y<400))    
        {
            return 10;
        }
        else if((x>220 & x<420)&&(y>280 && y<400))    
        {
            return 11;
        }
        else if((x>440 & x<640)&&(y>280 && y<400))    
        {
            return 12;
        }

    }

return 0;

}


photo_show_poin Electronic_Photo_Album_Init()      
{

    photo_show_poin psp =(photo_show_poin)malloc(sizeof(photo_show_node));    
        if(psp == NULL)
        {
            perror("malloc psp node failed");
            return (photo_show_poin)-1;
        }
    memset(psp,0,sizeof(photo_show_node));

    psp->page_sum  = 1;                                     
    psp->photo_sum = 0;                                   
    psp->photo_list_head.next = &(psp->photo_list_head);   
    psp->photo_list_head.prev = &(psp->photo_list_head);   

    if(Serach_Photo(psp) == -1)                       
    {
        printf("picture  search failed !\n");
        return (photo_show_poin)-1;
    }

return psp;

}


int Serach_Photo(photo_show_poin psp)           
{

    psp->dp = opendir(PHOTO_PATH);             
    if(psp->dp == NULL)
    {
        perror("opendir photo failed");
        return -1;
    }

while(1)                 
    {  
        struct dirent * eq = readdir(psp->dp);   
            if(eq == NULL) break;      
            
        if(eq->d_name[0] == '.') continue;
                                  
        if((eq->d_type == DT_REG) && (strcmp(&(eq->d_name[strlen(eq->d_name)-4]),".bmp") == 0))     
        {
            photo_link new_node = Create_Photo_List_Node();                    
            if(new_node==(photo_link)-1)
            {
                printf("创建新的图片节点失败！\n");
                return -1;
            }

            sprintf(new_node->photo_path,"%s/%s",PHOTO_PATH,eq->d_name);      

            Count_Page_Photo_Sum(psp);                                         

            if(Tail_Add_Photo_Node(psp,new_node) == -1)                           
            {                                                                   
                printf("add photo node failed!\n");
                return -1;
            }
        }
    }

    if(psp->photo_sum==0)psp->page_sum=0;          

    return 0;
}


photo_link Create_Photo_List_Node()         
{
    photo_link new_node=(photo_link)malloc(sizeof(photo_node));
    if(new_node==(photo_link)NULL)
    {
        perror("malloc failed");
        return (photo_link)-1;
    }
    memset(new_node,0,sizeof(photo_node));

    new_node->next=new_node;
    new_node->prev=new_node;

        return new_node;
}


int Count_Page_Photo_Sum(photo_show_poin psp)       
{
    psp->photo_sum++;                
    if((psp->photo_sum-1) % PAGE_SIZE == 0 && psp->photo_sum>PAGE_SIZE)
    {
        psp->page_sum++;             
    }

    return 0;
}


int Tail_Add_Photo_Node(photo_show_poin psp,photo_link new_node)    
{
    if(new_node == NULL)
    {
        printf("节点为空，尾插法失败！");
        return -1;
    }

      
    new_node->next = &(psp->photo_list_head);
    (psp->photo_list_head).prev->next = new_node;
    new_node->prev = (psp->photo_list_head).prev;
    (psp->photo_list_head).prev = new_node;

    new_node->photo_num = psp->photo_sum;    

    if(new_node->photo_num % 9 == 0)        
    {
        psp->page_sum = new_node->photo_num/9;
    }
    else
    {
        psp->page_sum = new_node->photo_num/9+1;
    }

return 0;
}


int Thumbnail_Ui(int page_num,photo_show_poin psp)              
{

    if(Show_Bmp("/function_bmp1.bmp")==-1)                      
    {
        printf("显示背景图失败!\n");
        return -1;
    }  

     if(Show_Bmp_Upper_Right("/out.bmp")==-1)    
    {
        printf("显示返回按钮失败!\n");
        return -1;
    }  

    int page_n=1;

    photo_link start_node = psp->photo_list_head.next;   

    for(int lp=0; lp< (page_num-1)*PAGE_SIZE; lp++)           
    {
        start_node = start_node->next;
    }

    int x=0,y=0;
    for(int lp=0; lp<PAGE_SIZE; lp++)
    {
        if(start_node == &(psp->photo_list_head)) break;
        Display_Photo(start_node->photo_path,4,x,y);             

        x+=220;                   
        if(x>600)
        {
            y+=140;
            x=0;
        }

        start_node = start_node->next;
        usleep(3000);
    }

return 0;
}


int Display_Photo(const char * bmp_file_path,int zoomout,int where_x,int where_y)   
{

    int bmp_fd = open(bmp_file_path,O_RDONLY);     
    if(bmp_fd == -1)
    {
        perror("open failed");
        return -1;
    }
    int lcd_fd=open(LCD_PATH,O_RDWR);
    if(lcd_fd==-1)
    {
        perror("open failed");
        return -1;
    }


    lseek(bmp_fd,18,SEEK_SET);
    int w=0;
    int h=0;
    read(bmp_fd,&w,sizeof(int));
    read(bmp_fd,&h,sizeof(int));

    int skip=0;

    if(w*3%4 !=0)
    {
        skip = 4-(w*3%4);
    }
    else
    {
        skip = 0;
    }
                          
    int bmp_color_size= (w * h * 3) + skip*LCD_H;
    char * bmp_color_p = (char *)malloc(bmp_color_size * sizeof(char));
    if(bmp_color_p == NULL)   
    {
        perror("malloc failed");
        return -1;
    }

    lseek(bmp_fd,54,SEEK_SET);     
    read(bmp_fd,bmp_color_p,bmp_color_size* sizeof(char));

    int  *lcd_mmap_star=(int *)mmap(NULL,              
                                    800*480*4,    
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED,
                                    lcd_fd,
                                    0);

int * tmp_lcd_mmap_start = lcd_mmap_star + LCD_W * where_y + where_x;


    for(int y=0,n=0; y<h/zoomout; y++)
    {
        for(int x=0; x<w/zoomout; x++,n=n+(3*zoomout))
        {
            *(tmp_lcd_mmap_start + w * (h/zoomout-1-y)+x) = 
            *(bmp_color_p+n)   << 0 |
            *(bmp_color_p+n+1) << 8 | 
            *(bmp_color_p+n+2) <<16;
        }

    n +=skip;
    if(zoomout!=1)
    {
        n += (w%zoomout)*3;
        
        n += ((w*(zoomout-1)*3)+(skip)*(zoomout-1));
    }

    }

    close(bmp_fd);
    close(lcd_fd);

    munmap(lcd_mmap_star,800*480*4);
    return 0;
}


int Show_character()
{

   show_charact();

    return 0;
}




int main()
{
    
    if(Show_Desktop("/window.bmp","/open.bmp")==-1)     
    {
        printf("显示桌面失败！\n");
        return -1;
    }

    return 0;
}















