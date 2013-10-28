//{{{
/*一个在awesome的面板上显示类似conky的系统数据程序。
  基本原理：在awesome3.0以上版本可以通过wibox的widget方便的显示一些自定义的信息，本程序既是这一功能的简单应用。
  基本的使用方法为：
  1、首先在awesome的配置文件中添加一个用于显示信息的widget:
  ----rc.lua-----
  --in wibox 定义自己的widget:
  conkytext = widget({ type = "textbox" })
  --将自定义的widget添加到面板中:
      mywibox[s].widgets = { 
        {   
            mylauncher,
            mytaglist[s],
            mypromptbox[s],
            layout = awful.widget.layout.horizontal.leftright
        },  
        mylayoutbox[s],
        mytextclock,
        s == 1 and mysystray or nil,
        conkytext,
        mytasklist[s],
        layout = awful.widget.layout.horizontal.rightleft
    }
  2、重启awesome，使用 echo "conkytext.text = \"hello\""|awesome-client 命令测试。
  3、在awesome中如果安装vicious或者awesome-extra包的话，还可以支持图片的显示以及集成的类似与conky的系统信息显示。
  4、为了简单，我没有再使用conky而是通过本程序自己取得各项系统数据并完成显示,天气数据是调用原来为conky写的一个程序实现，其余的数据
  都是通过本程序获取。
  5、所有数据都是通过/proc /sys这两个系统目录下获得的，由于版本不同可能数据所处位置稍有差异，请注意自己调正。
  					tybitsfox  2013-4-5
 *///}}}
#include"clsscr.h"
//定义监视的项目数量
#define	 jc			6
#define  chlen		1024
#define  mem_len	256
#define  weather	"/root/.conky/weather"
#define  wfile		"/tmp/wthdata.dat"
#define	 awesome	"awesome-client"
//电池电量获取所需
#define sfile   "/sys/class/power_supply/BAT1/uevent"
#define power_base  "POWER_SUPPLY_ENERGY_FULL="
//#define power_base  "POWER_SUPPLY_CHARGE_FULL="
#define power_now	"POWER_SUPPLY_ENERGY_NOW="
//#define power_now   "POWER_SUPPLY_CHARGE_NOW="
//CPU
#define cpu_file	"/proc/stat"
//memory
#define mem_file	"/proc/meminfo"
//net
#define net_updown	"/proc/net/dev"
//cpu temperature
#define cpu_temp	"/sys/class/thermal/thermal_zone0/temp"
//it's also be used on my thinkpad
//#define cpu_temp	"proc/acpi/ibm/thermal"
//定义显示信息及色彩
#define	 col_cyan	"#00ffff"
#define  col_red	"#ff0000"
#define  col_green  "#00ff00"
#define  col_blue	"#0000ff"
#define  col_yellow	"#ffff00"
#define	 col_dpink	"#ff00ff"
//定义临时文件名，用于确定进程的唯一性
#define  tmpfile	"/tmp/awemsg_tmp.dat"
#define  namefile	"/proc/%d/cmdline"
//信息显示的格式
//#define	 out_msg	"conkytext.text = \"<span color='%s'>| CPU:%s%s|内存:%s %s| 流量 ↓%s↑%s| 电量:%s| 泰安 %s %s |</span>\"\n"
#define	 out_msg	"| CPU:%s%s| 内存:%s %s| 流量 %s%s| 电量:%s| 泰安 %s %s | %s\n"  //2013-4-21添加天气
//macro define
#define	 zero(A)	memset(A,0,sizeof(A))
#define  sys_log(a,b)	openlog(a,LOG_PID,LOG_USER);syslog(LOG_NOTICE,b);closelog();
struct T_J
{
	int n;          //number of task
	char c;//[1024];	//command--no use now
};
struct T_J tj[jc]; 
//msg[5]=cpu,msg[3,4]=mem,msg[2]=bat,msg[1,0]=weather
/*msg数组用于保存不同的功能函数获取的待显示的数据，其中msg[0,1]保存了天气信息，msg[2]保存的电量,msg[3,4]保存的内存信息
 msg[5]保存cpu用户进程的使用率，msg[6,7]保存了网络数据流量，msg[8]保存cpu温度。
 */
char   msg[9][100];
char   fmt[chlen];
int	   cpu_v[4];//calc cpu avg
unsigned long long	   net_ud[2];//calc net flow
int chg_daemon(); //转为守护进程运行
void get_config();//因为没用配置文件，此函数改为初始化基本数据。
void format_msg(int i);//除了天气显示使用，其余的都在各自的函数独立实现了信息格式化。
int disp_msg();//完成后的信息输出
void get_batt();//battery get
void get_cpu();//cpu status
void get_mem();//mem status
void get_net();//net up/down
void set_unique(char *c);//unique
void get_temp();//cpu temperature

