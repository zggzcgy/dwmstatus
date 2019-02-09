#define _BSD_SOURCE

#define BATT_NOW "/sys/class/power_supply/BAT0/energy_now"
#define BATT_FULL "/sys/class/power_supply/BAT0/energy_full"
#define BATT_STATUS "/sys/class/power_supply/BAT0/status"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <X11/Xlib.h>

char *tzshanghai = "Asia/Shanghai";

static Display *dpy;

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

void
settz(char *tzname)
{
	setenv("TZ", tzname, 1);
}

char *
mktimes(char *fmt, char *tzname)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

	memset(buf, 0, sizeof(buf));
	settz(tzname);
	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL) {
		perror("localtime");
		exit(1);
	}

	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		exit(1);
	}

	return smprintf("%s", buf);
}

void
setstatus(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

char *
getbattery(void)
{
	long bnow = 0;
	long bfull = 0;
	char *status = malloc(sizeof(char)*16);
	char s = '?';
	FILE *fp = NULL;
	if ((fp = fopen(BATT_NOW, "r"))) {
		fscanf(fp, "%ld\n", &bnow);
		fclose(fp);
		fp = fopen(BATT_FULL, "r");
		fscanf(fp, "%ld\n", &bfull);
		fclose(fp);
		fp = fopen(BATT_STATUS, "r");
		fscanf(fp, "%s\n", status);
		fclose(fp);
		if (strcmp(status, "Charging") == 0)
			s = '+';
		else if (strcmp(status, "Discharging") == 0)
			s = '-';
		else if (strcmp(status, "Full") == 0)
			s = '=';
		free(status);
		return smprintf("%c%ld%%", s, (bnow/(bfull/100)));
	}
	free(status);
	return smprintf("%c", s);
}

char *
loadavg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0) {
		perror("getloadavg");
		exit(1);
	}

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}

int
main(void)
{
	char *status;
	char *avgs;
	char *batt;
	char *tmsh;

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(30)) {
		avgs = loadavg();
		batt = getbattery();
		tmsh = mktimes("%W %a %d %b %H:%M %Z %Y", tzshanghai);

		status = smprintf("%s %s %s",
				  avgs, batt, tmsh);
		setstatus(status);
		free(avgs);
		free(batt);
		free(tmsh);
		free(status);
	}

	XCloseDisplay(dpy);

	return 0;
}

