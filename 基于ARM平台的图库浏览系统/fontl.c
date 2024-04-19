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
int Draw_Point(int  where_x,int where_y,int color);//ָ������λ��ˢһ�����ص�
int Draw_Chinese(const char * fontls,int where_x,int where_y,int color);

int Init()
{
  //��LCD ���ֿ�
  I.lcd_fd = open(LCD_DEV_PATH,OPEN_FILE_FLAG);
  I.dzk_fd = open(DZK_FILE_PATH,OPEN_FILE_FLAG);
  if(I.lcd_fd == -1 || I.dzk_fd == -1)
  {
    perror("open file ");
    return -1;
  }

  I.dzk_size = lseek(I.dzk_fd,0,SEEK_END);//������ֿ�Ĵ�С�����ܸ������Ĵ�С��������ӳ��ռ�
  lseek(I.dzk_fd,0,SEEK_SET);//����ƫ����
  //ӳ��lcd��dzk
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

int Dis_Fontl(const char * fontl,int where_x,int where_y,int color)//fontl��Ŷ�ӦҪ��ʾ�ĺ���
{
  //���ж�ָ���������Ƿ����
  if(where_x+16 > 799)//�ж�x
  {
    //�ص�����
    where_x = 0;
    if(where_y+16 > 479)
    {
      printf("ָ�������������޷���ʾ��\n");
      return -1;
    }
    else
    {
      where_y+=16;
    }
  }

  //����lcd��ӳ��ָ��������ӦҪ��ʾ�����Ͻǵ�����λ��
  //int * new_lcd_p = I.lcd_star+(800*where_y)+where_x;
  I.dzk_offset = (94*(fontl[0]-1-0xa0)+(fontl[1]-1-0xa0))*32;
  char * new_dzk_p = I.dzk_star+I.dzk_offset;//����һ���µ�ָ��������dzk�ֿ��ӳ��ָ��
  //���Ҫ��ʾ�ĺ������ֿ��е�ƫ����
  //����ƫ������ӳ��ָ������Ҫ��ʾ�ĺ��ֶ�Ӧ��λ��
  int x,y,z;
  char type;//����һ���ַ��������ÿ���жϵ��ַ����ֿ�����

  for(y=0; y<16; y++)
  {
    for(x=0; x<2; x++)
    {
      type = *(new_dzk_p+2*y+x);//��ȡ�ֿ���һ���ֽڵĺ���ģ
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
    printf("ָ�������������޷���ʾ��\n");
    return -1;
  }

  *(I.lcd_star+800*where_y+where_x) = color;

  return 0;
}


int Draw_Chinese(const char * fontls,int where_x,int where_y,int color)
{
  //ѭ����ȡÿһ������
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

//  Dis_Fontl("��",400,240);//����İ�Ҫ���ֿ�����ı����ʽҪһ�£�GBK��
  Draw_Chinese("��һҳ��",650,200,0x00f0f0f0);
  Draw_Chinese("��һҳ��",650,325,0x00f0f0f0);
  Free();

  return 0;
}
