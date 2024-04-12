#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define SEARCH_MODE_PRESENT 1  //检索当前结点
#define SEARCH_MODE_BEFORE 2  //检索上一个结点 
#define BOOK_NAME_LEN 50
#define PASSWD  235687
#define PADDWD_LEN 6


typedef struct book_inf                  //书的信息
{
char book_name[BOOK_NAME_LEN];           //书名
int book_num;                            //书的数量
struct book_inf * next;
}node,*LINK;


int Administrator_System(LINK head_node);                       //管理员系统
int Administrator_Mode_Select(LINK head_node);                  //管理员模式选择
int User_System(LINK head_node);                                //用户系统
int User_Mode_Select(LINK head_node);                           //用户模式选择
// int Quit();                   //退出系统



LINK Creat_Book_Inf();                                          //创建新的结点，即创建新的书籍信息
int Tail_Add_Link_Node(LINK head_node);                         //尾插法添加书籍
LINK Search_Link_List_Node(LINK head_node,int search_mode);     //检索书籍
int Delete_BOOK(LINK head_node);                                //删除指定书籍 ,即删除结点 
int Modify_Book_Num(LINK head_node);                            //修改书籍数量
int Show_All_Book(LINK head_node);                              //显示全部书籍
int Lend_BOOK(LINK head_node);                                  //借阅书籍
int Return_Book(LINK head_node);                                     //归还书籍

LINK Creat_Book_Inf()                                                //创建新的结点，即创建新的书籍信息
{
    LINK new_node=(LINK)malloc(sizeof(node));
    if(new_node==(LINK)NULL)
    {
        printf("malloc failed");
        return (LINK)-1;
    }
    memset(new_node,0,sizeof(node));
    new_node->next=NULL;

    return new_node;
}


int Tail_Add_Link_Node(LINK head_node)                               //尾插法添加书籍
{
    if(head_node==(LINK) NULL)
    {
        printf("头结点异常！\n");
        return -1;
    }
    //先创建新节点并且赋值数据
    LINK new_node=Creat_Book_Inf();
    if(new_node==(LINK)-1)
    {
        printf("添加书籍失败！\n");
    }
    
    memset(new_node,0,sizeof(node));

    printf("请输入新书籍的名字！\n");
    scanf("%s",new_node->book_name);
    while(getchar()!='\n');
    printf("请输入新书籍数量！\n");
    scanf("%d",&(new_node->book_num));

    LINK end_node;     //拿到最后一个结点
    for(end_node=head_node;end_node->next!=NULL;end_node=end_node->next);
    //现在end_node就是最后一个结点了

    end_node->next=new_node;

    printf("添加新书籍成功,书籍名字为:《%s》,书籍数量为:%d\n",new_node->book_name,new_node->book_num);

    return 0;
}


LINK Search_Link_List_Node(LINK head_node,int search_mode)           //检索书籍
{

    if(head_node==(LINK)NULL)
    {
        printf("头节点异常\n");
        return (LINK)-1;
    }
    else if(head_node->next==(LINK)NULL)
    {
        printf("没有一本书籍！\n");
        return (LINK)0;
    }
    char book_name[BOOK_NAME_LEN];
    printf("请输入要操作的书籍名字！\n");
    scanf("%s",book_name);
    while(getchar()!='\n');

    for(LINK tmp_node=head_node;tmp_node->next!=NULL;tmp_node=tmp_node->next)
    {
        if(strcmp(tmp_node->next->book_name,book_name)==0)
        {

            printf("找到书籍，该书籍还有%d本！\n",tmp_node->next->book_num);

            if(search_mode==SEARCH_MODE_PRESENT)
            {
                return tmp_node->next;
            }
            if(search_mode==SEARCH_MODE_BEFORE)
            {
                return tmp_node;
            }
        }
    }

    printf("找不到该书籍，本图书馆没有该书籍!\n");
    return (LINK)0;
}


int Delete_BOOK(LINK head_node)                                      //删除指定书籍  
{
    if(head_node==(LINK)NULL)
    {
        printf("头结点异常\n");
        return -1;
    }
    else if(head_node->next==NULL)
    {
        printf("图书馆中没有一本书，无可删除书籍！\n");
        return 0;
    }
    LINK before_node=Search_Link_List_Node(head_node,SEARCH_MODE_BEFORE);
    if(before_node==(LINK)-1)                           //返回的是要删除的结点的上一个结点
    {
        printf("程序异常,无法删除！\n");
        return -1;
    }
    else if(before_node==(LINK)0)
    {
        printf("没有找到该书籍信息\n");
        return 0;
    }
    else
    {
        LINK del_node     =before_node->next;
        before_node->next =del_node->next;
        del_node->next    =NULL;
        printf("删除书籍成功\n");
    }

    return 0;
}


int Modify_Book_Num(LINK head_node)                                   //修改书籍数量
{
    if(head_node==(LINK)NULL)
    {
        printf("头结点异常\n");
        return -1;
    }
    if(head_node->next==NULL)
    {
        printf("图书馆中无书,无法修改书籍数量！\n");
        return 0;
    }
    else
    {

        //先找到这个书籍，再修改数量
        LINK Obj_Book_Before=Search_Link_List_Node(head_node,SEARCH_MODE_BEFORE);    //返回的是上一个结点
        if(Obj_Book_Before==(LINK)-1)
        {
            printf("程序异常，无法修改!\n");
            return -1;
        }
        else if(Obj_Book_Before==(LINK)0)
        {
            printf("找不到该书籍，无法修改书籍数量！\n");
            return 0;
        }
        else    //找到了该书籍的上一个结点
        {

            printf("书籍原先的数量是%d本，请输入要修改的数量。\n",Obj_Book_Before->next->book_num);
            int num=0;
            scanf("%d",&num);

            Obj_Book_Before->next->book_num=num;

            printf("修改数量成功！\n");

        }
    }
    return 0;
}


int Show_All_Book(LINK head_node)                                     //显示全部书籍
{
    if(head_node==(LINK)NULL)
    {
        printf("头结点异常\n");
        return -1;
    }
    if(head_node->next==NULL)
    {
        printf("图书馆中没有书籍！\n");
        return 0;
    }
    else
    {
        for(LINK tmp_node=head_node->next;tmp_node!=NULL;tmp_node=tmp_node->next)
        {    //这样就可以遍历头结点后面的每一个结点

            printf("书籍名字:《%s》, 书籍数量：%d。\n",tmp_node->book_name,tmp_node->book_num);

        }
    }
return 0;
}


int Administrator_Mode_Select(LINK head_node)                        //管理员模式选择
{

    int num=0;
    while(1)
   {
        printf("请选择要进行的操作！\n");

        printf("⚪⚪⚪⚪1 添加书籍！！！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪2 删除书籍！！！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪3 修改书籍数量！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪4 检索书籍信息！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪5 显示全部书籍！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪6 退出管理员模式⚪⚪⚪⚪\n");

        scanf("%d",&num);

        switch(num)
        {
         case 1:Tail_Add_Link_Node(head_node);                         break;    //尾插法添加书籍信息
         case 2:Delete_BOOK(head_node);                                break;    //删除结点，即删除书籍信息
         case 3:Modify_Book_Num(head_node);                            break;    //修改书籍数量
         case 4:Search_Link_List_Node(head_node,SEARCH_MODE_PRESENT);  break;    //检索书籍
         case 5:Show_All_Book(head_node);                              break;    //显示全部书籍
         case 6:return 0;                                              break;    //退出管理员模式
         default:printf("无指令，请重新输入！\n");                       break;
         }


        printf("hahah\n");
        sleep(2);
        system("clear");

     }

    return 0;

};


int Administrator_System(LINK head_node)                             //管理员系统
{

    // char passwd[PADDWD_LEN+1];    //管理员的密码是宏定义的
  
    // scanf("%s",passwd);

    int passwd=0;
    printf("请输入管理员密码\n");
    scanf("%d",&passwd);

    if(passwd==PASSWD)
    {
        printf("欢迎进入管理员系统！\n");
        
        //进入管理员模式选择
        Administrator_Mode_Select(head_node);
    }

    else
    {
     printf("密码输入错误！退出\n");
     return 0;
    }

    return 0;
}


int User_System(LINK head_node)                                     //用户系统
{
    printf("欢迎进入学生系统！\n");

    //进入学生选择模式
    User_Mode_Select(head_node);

    return 0;

}


int User_Mode_Select(LINK head_node)                              //用户模式选择
{
    int num=0;
    while(1)
   {
        printf("请选择要进行的操作！\n");

        printf("⚪⚪⚪⚪1 借阅书籍！！！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪2 归还书籍！！！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪3 检索书籍信息！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪4 显示全部书籍！⚪⚪⚪⚪\n");
        printf("⚪⚪⚪⚪5 退出用户模式！⚪⚪⚪⚪\n");

        scanf("%d",&num);

        switch(num)
        {
         case 1:Lend_BOOK(head_node);                                       break;          //借阅书籍，即找到指定的书籍然后把书籍数量-1
         case 2:Return_Book(head_node);                                     break;          //归还书籍，即找到指定的书籍然后把书籍数量+1
         case 3:Search_Link_List_Node(head_node,SEARCH_MODE_PRESENT);       break;          //检索书籍信息
         case 4:Show_All_Book(head_node);                                   break;          //显示全部书籍   
         case 5:return 0;                                                   break;          //退出用户模式
         default:printf("无指令，请重新输入！\n");                           break;
         }

        printf("hahah\n");
        sleep(2);
        system("clear");

     }

    return 0;









}


int Lend_BOOK(LINK head_node)                                    //借阅书籍
{
    if(head_node==(LINK)NULL)
    {
        printf("头结点异常！\n");
        return -1;
    }
    else if(head_node->next==NULL)
    {
        printf("图书馆中没有一本书，无法借阅！\n");
        return 0;
    }
    else                  
    {
        LINK Obj_Book_PRESENT=Search_Link_List_Node(head_node,SEARCH_MODE_PRESENT);        //返回的是当前的结点
        if(Obj_Book_PRESENT==(LINK)-1) 
        {
            return -1;
        }
        else if(Obj_Book_PRESENT==(LINK)0)         
        {
            printf("没有数据\n");
            return 0;
        }
        else
        {
            printf("请输入你要借阅的书籍的名字！\n");
            char Lend_Book[BOOK_NAME_LEN];
            scanf("%s",Lend_Book);

            printf("请输入你要借阅的书籍的数量！\n");
            int num=0;
            scanf("%d",&num);

            if(strcmp(Obj_Book_PRESENT->book_name,Lend_Book)==0)
            {
                Obj_Book_PRESENT->book_num=Obj_Book_PRESENT->book_num-num;
                printf("借阅书籍成功！\n");
            }
            else
            {
                 printf("图书馆中没有该书籍，借阅书籍失败！\n");
            }
           
        }
    }

    return 0;
}


int Return_Book(LINK head_node)                                      //归还书籍
{

    if(head_node==(LINK)NULL)
    {
        printf("头结点异常！\n");
        return -1;
    }
    else if(head_node->next==NULL)
    {
        printf("图书馆中没有这本书，无法归还！\n");
        return 0;
    }
    else                       
    {
        LINK Obj_Book_PRESENT=Search_Link_List_Node(head_node,SEARCH_MODE_PRESENT);        //返回的是当前的结点
        if(Obj_Book_PRESENT==(LINK)-1) 
        {
            return -1;
        }
        else if(Obj_Book_PRESENT==(LINK)0)         
        {
            printf("没有数据\n");
            return 0;
        }
        else
        {
            printf("请输入你要归还的书籍的名字！\n");
            char Lend_Book[BOOK_NAME_LEN];
            scanf("%s",Lend_Book);

            printf("请输入你要归还的书籍的数量！\n");
            int num=0;
            scanf("%d",&num);

            if(strcmp(Obj_Book_PRESENT->book_name,Lend_Book)==0)
            {
                Obj_Book_PRESENT->book_num=Obj_Book_PRESENT->book_num+num;
                printf("归还书籍成功！\n");
            }
            else
            {
                 printf("图书馆中没有该书籍，借阅书籍失败！\n");
            }
           
        }
    }
    return 0;
}


int main()
{


    LINK head_node=Creat_Book_Inf();     //先创建头结点
     if(head_node==(LINK)NULL)
     {
         printf("初始化失败！\n");   
     }

    while(1)
    {
    printf("请选择要进入的模式：1、管理员模式 2、学生模式\n");
    int num=0;
    scanf("%d",&num);
    switch(num)
    {
        case 1:Administrator_System(head_node);             break;
        case 2:User_System(head_node);                      break; 
        // case 3:Quit();                                   break;
        default:printf("无指令，请重新输入！\n");            break;
    }

    }

    return 0;
}