#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LCD_DEV_PATH  "/dev/fb0"
#define DZK_FILE_PATH "/16_chinese.DZK"
#define MAP_LENGTH 800*480*4
#define MAP_OFFSET 0
#define OPEN_FILE_FLAG O_RDWR


struct inf
{
  int  lcd_fd,dzk_fd;
  int  dzk_size;
  int  *lcd_star;
  int  dzk_offset;
  int  color;
  char *dzk_star;
}I;

int Init();
int Free();
int Dis_Fontl(const char * fontl,int where_x,int where_y,int color);
int Draw_Point(int  where_x,int where_y,int color);//指定任意位置刷一个像素点
int Draw_Chinese(const char * fontls,int where_x,int where_y,int color);

int Init()
{
  //打开LCD 汉字库
  I.lcd_fd = open(LCD_DEV_PATH,OPEN_FILE_FLAG);
  I.dzk_fd = open(DZK_FILE_PATH,OPEN_FILE_FLAG);
  if(I.lcd_fd == -1 || I.dzk_fd == -1)
  {
    perror("open file ");
    return -1;
  }

  I.dzk_size = lseek(I.dzk_fd,0,SEEK_END);//求出汉字库的大小，才能根据他的大小进行申请映射空间
  lseek(I.dzk_fd,0,SEEK_SET);//重置偏移量
  //映射lcd和dzk
  I.lcd_star = mmap(NULL,MAP_LENGTH,PROT_READ | PROT_WRITE,MAP_SHARED,I.lcd_fd,MAP_OFFSET);
  I.dzk_star  = mmap(NULL,I.dzk_size,PROT_READ | PROT_WRITE,MAP_SHARED,I.dzk_fd,MAP_OFFSET);
  if(I.lcd_star == MAP_FAILED || I.dzk_star == MAP_FAILED)
  {
    perror("mmap lcd ... mmap dzk ");
    return -1;
  }

  return 0;
}

int Free()
{
  close(I.lcd_fd);
  close(I.dzk_fd);
  munmap(I.lcd_star,MAP_LENGTH);
  munmap(I.dzk_star,I.dzk_size);

  return 0;
}

int Dis_Fontl(const char * fontl,int where_x,int where_y,int color)//fontl存放对应要显示的汉字
{
  //先判断指定的坐标是否可用
  if(where_x+16 > 799)//判断x
  {
    //回到行首
    where_x = 0;
    if(where_y+16 > 479)
    {
      printf("指定的坐标有误，无法显示！\n");
      return -1;
    }
    else
    {
      where_y+=16;
    }
  }

  //先让lcd的映射指针跳到对应要显示的左上角的坐标位置
  //int * new_lcd_p = I.lcd_star+(800*where_y)+where_x;
  I.dzk_offset = (94*(fontl[0]-1-0xa0)+(fontl[1]-1-0xa0))*32;
  char * new_dzk_p = I.dzk_star+I.dzk_offset;//定义一个新的指针变量存放dzk字库的映射指针
  //求出要显示的汉字在字库中的偏移量
  //根据偏移量把映射指针跳到要显示的汉字对应的位置
  int x,y,z;
  char type;//定义一个字符变量存放每次判断的字符汉字库数据

  for(y=0; y<16; y++)
  {
    for(x=0; x<2; x++)
    {
      type = *(new_dzk_p+2*y+x);//获取字库中一个字节的汉字模
      for(z=0; z<8; z++)
      {

        if(type &(0x80 >> z))
        {
          //*(new_lcd_p+800*y+(8*x+z)) = 0x0000ff00;
          Draw_Point(where_x+(8*x+z),(where_y+y),color);
        }
        else
        {
            Draw_Point(where_x+(8*x+z),(where_y+y),0x00000000);
        }
      }
    }
  }
  return 0;
}

int Draw_Point(int  where_x,int where_y,int color)
{
  if(where_x > 799 || where_y >479)
  {
    printf("指定的坐标有误，无法显示！\n");
    return -1;
  }

  *(I.lcd_star+800*where_y+where_x) = color;

  return 0;
}


int Draw_Chinese(const char * fontls,int where_x,int where_y,int color)
{
  //循环截取每一个汉字
  const char * new_f = fontls;
  int n;
  for(n=0; n<(strlen(fontls)/2); n++)
  {

    Dis_Fontl(new_f,where_x,where_y,color);
    new_f+=2;
    where_x+=18;
  }

  return 0;
}


int show_charact()
{
  
  Init();

//  Dis_Fontl("啊",400,240);//这里的啊要和字库的中文编码格式要一致（GBK）
  Draw_Chinese("上一页！",650,200,0x00f0f0f0);
  Draw_Chinese("下一页！",650,325,0x00f0f0f0);
  Free();

  return 0;
}
