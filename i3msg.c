/*本程序用于获取在awesome的panel上输入显示信息，信息的获得使用原conky中个人编写的取得当地天气预报的数据
 在添加部分系统监视数据，定时一并输出显示到panel上。
 						tybitsfox  2013-4-3
 */
#include<time.h>
#include"awemsg.h"
//{{{ int main(int argc,char** argv)
int main(int argc,char** argv)
{
	char* av[]={weather,0};
	int i,j,job[jc];
	static int k=0;
	set_unique(argv[0]);
	//if(chg_daemon()!=0)
	//	exit(0);
	//sys_log(argv[0],"awesome_panel write tool ready\n");
	get_config();
	//为保证正常获取，执行三次
	/*for(i=0;i<3;i++)
	{
		if(system(weather)==-1)
		{
			sys_log(argv[0],"error to get weather\n");
			sleep(1);
			continue;
		}
		else
			break;
	}*/
	format_msg(0);//初次运行时获取天气
	get_batt();// 初次运行时获取电量
	get_cpu();//cpu
	get_mem();//mem
	get_net();//net
	get_temp();//temperature
	disp_msg();
	/*
	if(disp_msg()==0)
	{
		sys_log(argv[0],fmt);
	}	
	else
	{
		sys_log(argv[0],"show messge error\n");
		exit(0);
	}
	//return 0;
	for(i=0;i<jc;i++)
		job[i]=0;
	while(1)
	{
		sleep(2);
		for(i=0;i<jc;i++)
		{
			job[i]++;
			switch(i)
			{
				case 0://job 1 获取天气数据
					if(job[0]>=tj[0].n)
					{
						job[0]=0;
						system(weather);
						format_msg(0);
						k=1;
					}
					break;
				case 1: //job2-battery 电池电量
					if(job[1]>=tj[1].n)
					{
						job[1]=0;
						get_batt();
						//format_msg(1); nouse here
						k=1;
					}
					break;
				case 2://cpu 
					if(job[2]>=tj[2].n)
					{
						job[2]=0;
						get_cpu();
						k=1;
					}
					break;
				case 3://mem
					if(job[3]>=tj[3].n)
					{
						job[3]=0;
						get_mem();
						k=1;
					}
					break;
				case 4://net
					if(job[4]>=tj[4].n)
					{
						job[4]=0;get_net();k=1;
					}
					break;
				case 5:// temperature
					if(job[5]>=tj[5].n)
					{
						job[5]=0;get_temp();k=1;
					}
					break;
			};
		}
		if(k)
		{
			k=0;
			if(disp_msg()!=0)
			{
				sys_log(argv[0],"show messge error#1\n");
				exit(0);
			}
		}
	}
	sys_log(argv[0],"now testing over...\n");
	*/
	exit(0);
}//}}}
//{{{ int chg_daemon()
int chg_daemon()
{
	int i,pid;
	pid=fork();
	if(pid!=0)
		return 1;
	signal(SIGHUP,SIG_IGN);
	if(setsid()<0)
		return 1;
	pid=fork();
	if(pid!=0)
		return 1;
	chdir("/");/*
	for(i=0;i<64;i++)
		close(i);
	open("/dev/null",O_RDONLY);
	open("/dev/null",O_RDWR);
	open("/dev/null",O_RDWR);*/
	return 0;
}//}}}
//{{{ void get_config()
void get_config()
{
	int i;
//初始化所有指针，句柄，描述字
	tj[0].n=3601; //0为天气的资料获取索引，轮寻时间为3600秒、1小时
	tj[1].n=15;  //1为电池电量的获取索引，轮寻时间为20秒
	tj[2].n=3; //2为CPU频率获取索引，轮寻时间为6秒
	tj[3].n=3; //3为内存使用索引,轮寻时间6秒
	tj[4].n=4; //4为网络信息索引，论寻时间8秒
	tj[5].n=4; //5为cpu温度的索引，轮询时间8秒
	for(i=0;i<4;i++)
		cpu_v[i]=0;
	net_ud[0]=0;net_ud[1]=0;
	return;
}
//}}}
//{{{ void format_msg(int i)
void format_msg(int i)
{
	FILE *f;
	int m;
	char buf1[chlen];
	if(i==0)
	{
		memset(msg[i],0,100);
		memset(msg[i+1],0,100);
		f=fopen(wfile,"r");
		if(f==NULL)
		{
			memcpy(msg[0],"Unknow",6);
			memcpy(msg[1],"Unknow",6);
			return;
		}
		fgets(msg[0],sizeof(msg[0]),f);//温度
		m=strlen(msg[0]);
		if(msg[0][m-1]=='\n')
			msg[0][m-1]=0;
		fgets(msg[1],sizeof(msg[1]),f);//今日天气
		m=strlen(msg[1]);
		if(msg[1][m-1]=='\n')
			msg[1][m-1]=0;
		fclose(f);
		return;
	}
	return;
}
//}}}
//{{{ int disp_msg()
int disp_msg()
{
	//FILE *f;
	struct tm *p;
	time_t txm;
	int i;
	char ch[100];
	zero(ch);
	time(&txm);
	p=gmtime(&txm);
	i=p->tm_hour+8;
	if(i>23)
		i-=24;
	snprintf(ch,sizeof(ch),"%d年%d月%d日 %d时%d分",p->tm_year+1900,p->tm_mon+1,p->tm_mday,i,p->tm_min);
	//f=popen(awesome,"w");
	//if(f==NULL)
	//	return 1;
	memset(fmt,0,chlen);
	//snprintf(fmt,chlen,out_msg,col_yellow,msg[5],msg[8],msg[4],msg[3],msg[6],msg[7],msg[2],msg[1],msg[0]);
	snprintf(fmt,chlen,out_msg,msg[5],msg[8],msg[4],msg[3],msg[6],msg[7],msg[2],msg[1],msg[0],ch);
	printf(fmt);
	//fputs(fmt,f);
	//pclose(f);
	return 0;
}//}}}
//{{{ void get_batt();//battery get
void get_batt()//battery get
{
    float ft;
    FILE *f;
    int i,j,k,l;
    char *c,ch[512];
	memset(msg[2],0,100);
    f=fopen(sfile,"r");
    if(f==NULL)
    {
		memcpy(msg[2],"55",3);
        return ;
    }
    i=j=0;k=0;
    memset(ch,0,sizeof(ch));
    while(fgets(ch,sizeof(ch),f)!=NULL)
    {
        c=strstr(ch,power_base);
        if(c!=NULL)
        {
            c+=strlen(power_base);
            i=atoi(c);
            k++;
            goto  lop1;
        }
        c=strstr(ch,power_now);
        if(c!=NULL)
        {
            c+=strlen(power_now);
            j=atoi(c);
            k++;
        }
lop1:
        memset(ch,0,sizeof(ch));
        if(k>=2)
            break;
    }
    fclose(f);
    ft=(float)j/i;
    snprintf(msg[2],100,"%2.0f%%℃ ",ft*100);
    return ;

}//}}}
//{{{ void get_cpu()
void get_cpu()
{
	float fot;
	FILE *f;
	int len,i,j,k[4],t[4],l;
	char ch[mem_len],*c1,*c2,buf[10];
	for(i=0;i<4;i++)
	{
		k[i]=0;cpu_v[i]=0;
	}
	f=fopen(cpu_file,"r");
	if(f==NULL)
	{
		snprintf(msg[5],100,"Unknow");
		return;
	}
	memset(ch,0,mem_len);
	fgets(ch,mem_len,f);
	fclose(f);
	c1=ch;len=strlen(ch);
	for(i=0;i<4;i++)
	{
		len=strlen(c1);
		for(j=0;j<len;j++)
		{
			if(c1[j]>=0x30 && c1[j]<=0x39)
			{
				l=1;
				while(c1[j+l]!=' ')
				{
					l++;
					if(l>=(len-j))
					{
						l=-1;
						break;
					}
				}
				if(l==-1)
				{
					snprintf(msg[5],100,"ooo");
					return;
				}
				memset(buf,0,10);
				memcpy(buf,&c1[j],l);
				cpu_v[i]=atoi(buf);
				c2=c1+l+j;
				c1=c2;
				break;
			}
		}
	}
	sleep(1);
	memset(msg[5],0,100);
	f=fopen(cpu_file,"r");
	if(f==NULL)
	{
		snprintf(msg[5],100,"Unknow");
		return;
	}
	memset(ch,0,mem_len);
	fgets(ch,mem_len,f);
	fclose(f);
	c1=ch;len=strlen(ch);
	for(i=0;i<4;i++)
	{
		len=strlen(c1);
		for(j=0;j<len;j++)
		{
			if(c1[j]>=0x30 && c1[j]<=0x39)
			{
				l=1;
				while(c1[j+l]!=' ')
				{
					l++;
					if(l>=(len-j))
					{
						l=-1;
						break;
					}
				}
				if(l==-1)
				{
					snprintf(msg[5],100,"ooo");
					return;
				}
				memset(buf,0,10);
				memcpy(buf,&c1[j],l);
				k[i]=atoi(buf);
				c2=c1+l+j;
				c1=c2;
				break;
			}
		}
	}
	for(i=0;i<4;i++)
	{
		t[i]=k[i]-cpu_v[i];
		cpu_v[i]=k[i];
	}
	fot=(float)t[0]/(t[0]+t[1]+t[2]+t[3]);
	snprintf(msg[5],100,"%2.0f%% ",fot*100);
	return;
}//}}}
//{{{ void get_mem()
void get_mem()
{
	FILE *file;
	int a,i,j,k[2],l;
	char c[2][100],buf[20],*c1,*c2,ch[100];
	float fot;
	memset(msg[3],0,100);
	memset(msg[4],0,100);
	file=fopen(mem_file,"r");
	if(file==NULL)
	{
		snprintf(msg[3],100,"xx");
		snprintf(msg[4],100,"oo");
		return;
	}
	for(i=0;i<6;i++)
	{
		memset(ch,0,100);
		fgets(ch,100,file);
		if(i==0)
			memcpy(c[0],ch,100);
		if(i==5)
			memcpy(c[1],ch,100);
	}
	fclose(file);
	for(a=0;a<2;a++)
	{
		l=strlen(c[a]);c1=c[a];
		for(i=0;i<l;i++)
		{
			if(c[a][i]>=0x30 && c[a][i]<=0x39)
			{
				c1=c[a]+i;
				break;
			}
		}
		l=strlen(c1);j=0;
		for(i=0;i<l;i++)
		{
			if(c1[i]==' ')
			{
				j=1;
				memset(buf,0,20);
				memcpy(buf,c1,i);
				k[a]=atoi(buf);
				break;
			}
		}
		if(j==0)
		{
			snprintf(msg[3],100,"xx");
			snprintf(msg[4],100,"oo");
			return;
		}
	}
	fot=(float)k[1]/1024;
	snprintf(msg[4],100,"%0.1fMb ",fot);
	fot=(float)k[1]/k[0];
	snprintf(msg[3],100,"%0.2f%% ",fot*100);
	return;
}//}}}
//{{{ void get_net()
void get_net()
{//取得文件：/proc/net/dev,数据位于第四行，需要计算两次访问之间字节差以获得进出的流量 1,9
	FILE *file;
	int i,j,k,l,m[2],n[2];
	float fot;
	char *c1,*c2,ch[300],buf[20];
	for(i=0;i<2;i++)
		net_ud[i]=0;
	file=fopen(net_updown,"r");
	if(file==NULL)
	{
		snprintf(msg[6],100,"00");
		snprintf(msg[7],100,"00");
		return ;
	}
	for(i=0;i<4;i++)
	{
		memset(ch,0,300);
		fgets(ch,300,file);
	}
	fclose(file);c1=ch;k=0;
	l=strlen(c1);
	for(j=0;j<l;j++)
	{
		if(c1[j]>=0x30 && c1[j]<=0x39)
		{
			if(k==1)
			{
				c2=c1+j;i=0;
				while(c2[i]>=0x30 && c2[i]<=0x39)
					i++;
				memset(buf,0,20);
				memcpy(buf,c2,i);
				net_ud[0]=atoi(buf);
				j+=i;
				k++;
			}
			else
			{
				if(k==9)
				{
					c2=c1+j;i=0;
					while(c2[i]>=0x30 && c2[i]<=0x39)
						i++;
					memset(buf,0,20);
					memcpy(buf,c2,i);
					net_ud[1]=atoi(buf);
					break;
				}
				if(k!=9)
				{
					c2=c1+j;i=0;
					while(c2[i]>=0x30 && c2[i]<=0x39)
						i++;
					j+=i;k++;
				}
			}
		}
	}
	sleep(1);
	memset(msg[6],0,100);
	memset(msg[7],0,100);
	for(i=0;i<2;i++)
	{
		m[i]=0;n[i]=0;
	}
	file=fopen(net_updown,"r");
	if(file==NULL)
	{
		snprintf(msg[6],100,"00");
		snprintf(msg[7],100,"00");
		return ;
	}
	for(i=0;i<4;i++)
	{
		memset(ch,0,300);
		fgets(ch,300,file);
	}
	fclose(file);c1=ch;k=0;
	l=strlen(c1);
	for(j=0;j<l;j++)
	{
		if(c1[j]>=0x30 && c1[j]<=0x39)
		{
			if(k==1)
			{
				c2=c1+j;i=0;
				while(c2[i]>=0x30 && c2[i]<=0x39)
					i++;
				memset(buf,0,20);
				memcpy(buf,c2,i);
				m[0]=atoi(buf);
				j+=i;
				k++;
			}
			else
			{
				if(k==9)
				{
					c2=c1+j;i=0;
					while(c2[i]>=0x30 && c2[i]<=0x39)
						i++;
					memset(buf,0,20);
					memcpy(buf,c2,i);
					m[1]=atoi(buf);
					break;
				}
				if(k!=9)
				{
					c2=c1+j;i=0;
					while(c2[i]>=0x30 && c2[i]<=0x39)
						i++;
					j+=i;k++;
				}
			}
		}
	}
	n[0]=m[0]-net_ud[0];
	n[1]=m[1]-net_ud[1];
	net_ud[0]=m[0];net_ud[1]=m[1];
	fot=(float)n[0]/1024;
	snprintf(msg[6],100,"%0.1fKB ",fot);
	fot=(float)n[1]/1024;
	snprintf(msg[7],100,"%0.1fKB ",fot);
	return;
}//}}}
//{{{ void set_unique(char *c)
void set_unique(char *c)
{
	int fd1,fd2,i,j,k;
	char buf[100],cmd[100],*ch;
	pid_t pid;
	pid=getpid();
	zero(cmd);
	ch=strrchr(c,'/');
	if(ch==NULL)
	{
		sys_log(c,"strrchr error\n");
		return;
	}
	memcpy(cmd,ch,strlen(ch));
	//ch=getenv("_");
	memcpy(cmd,c,strlen(c));//save path name
	fd1=open(tmpfile,O_RDONLY|O_CREAT,0644);
	if(fd1<0)
	{
		sys_log(c,"open or create tmpfile error\n");
		return;
	}
	zero(buf);
	i=read(fd1,buf,sizeof(buf));
	close(fd1);
	if(i<=1)//create new
		goto rep_1;
	k=atoi(buf);
	if(k==pid)
		return;
	zero(buf);
	snprintf(buf,sizeof(buf),namefile,k);
	fd2=open(buf,O_RDONLY);
	if(fd2<0)
	{
		if(errno==2)//没有该进程id对应的文件，该进程已退出
			return;
		sys_log(c,"get proc file error\n");
		return;
	}
	zero(buf);
	i=read(fd2,buf,sizeof(buf));
	close(fd2);
	if(strstr(buf,cmd)!=NULL)
	{//find it
		kill((pid_t)k,9);
	}
rep_1:		
	fd1=open(tmpfile,O_RDWR|O_TRUNC);
	if(fd1<0)
	{
		sys_log(c,"open file error\n");
		return;
	}
	zero(buf);
	snprintf(buf,sizeof(buf),"%d",pid);
	write(fd1,buf,strlen(buf));
	close(fd1);
	return;
}//}}}
//{{{ void get_temp()
void get_temp()
{
	int i,j,k;
	FILE *file;
	char buf[100];
	zero(msg[8]);
	file=fopen(cpu_temp,"r");
	if(file==NULL)
	{
		snprintf(msg[8],sizeof(msg[8]),"00 ");
		return;
	}
	zero(buf);
	fgets(buf,sizeof(buf),file);
	fclose(file);
	i=atoi(buf);
	snprintf(msg[8],sizeof(msg[8]),"%d℃ ",i/1000);
	return;
}//}}}








