/*  *	WRay_Message.c * *	QuickDraw 3D 1.6 Sample *	Robert Dierkes * *	 07/28/98	RDD		Created. *//*------------------*//*	Include Files	*//*------------------*/#include "QD3D.h"#include "QD3DErrors.h"#include <string.h>#ifdef USE_DEBUGGING	#define	LIBRARY_ERROR	0	/**/	#define	SECRET_ERROR	1	/**/#endif /* USE_DEBUGGING *//*------------------*//*	Include Files	*//*------------------*/#if	defined(LIBRARY_ERROR) && (LIBRARY_ERROR)#include "Error_Lib.h"#endif /* LIBRARY_ERROR */#include "WRay_Message.h"#if	defined(SECRET_ERROR) && (SECRET_ERROR)/*----------------------*//*	Local Prototypes	*//*----------------------*/static void Message_Concatenate(	char	*typeStr,	long	value,	char	*message,	char	*outputMessage);static void Message_SecretError(	TQ3Error	error,	char		*message);static void Message_SecretNotice (	TQ3Notice	notice,	char		*message);static void Message_SecretWarning(	TQ3Warning	warning,	char		*message);typedef void (*EtDevelopmentErrorFunc)(	TQ3Error		error,	char 		*message);void EiDevelopmentError_Register(	EtDevelopmentErrorFunc		errorFunc);typedef void (*EtDevelopmentNoticeFunc)(	TQ3Notice	notice,	char 		*message);void EiDevelopmentNotice_Register(	EtDevelopmentNoticeFunc		noticeFunc);typedef void (*EtDevelopmentWarningFunc)(	TQ3Warning	warning,	char 		*message);void EiDevelopmentWarning_Register(	EtDevelopmentWarningFunc	warningFunc);#endif /* SECRET_ERROR */void Message_Register(void){#if	defined(LIBRARY_ERROR) && (LIBRARY_ERROR)	InstallDefaultErrorHandler();	InstallDefaultWarningHandler();	InstallDefaultNoticeHandler();#endif /* LIBRARY_ERROR */#if	defined(SECRET_ERROR) && (SECRET_ERROR)	EiDevelopmentError_Register  (NULL);	EiDevelopmentNotice_Register (NULL);	EiDevelopmentWarning_Register(NULL);	EiDevelopmentError_Register  (Message_SecretError);	EiDevelopmentNotice_Register (Message_SecretNotice);	EiDevelopmentWarning_Register(Message_SecretWarning);#endif /* SECRET_ERROR */}#if	defined(SECRET_ERROR) && (SECRET_ERROR)static void Message_Concatenate(char *typeStr, long value, char *message, char *outputMessage){	char	*slash, *period, *start;	start = message;	/* Find first period in file name at end of path name */	period = strchr(message, '.');	if (period != NULL) {		*period = '\0';		/* Find last slash in path name */		slash = strrchr(message, '/');		if (slash != NULL) {			/* No path name */			start = slash+1;		}		*period = '.';	}	sprintf (outputMessage, "QD3D %s %ld: %s", typeStr, value, start);}static void Message_SecretError(TQ3Error error, char *message){	char	outputMessage[512];	Message_Concatenate("Error", error, message, outputMessage);#if defined(OS_MACINTOSH) && OS_MACINTOSH	debugstr(outputMessage);#endif}static void Message_SecretNotice(TQ3Notice notice, char *message){	char	outputMessage[512];	Message_Concatenate("Notice", notice, message, outputMessage);#if defined(OS_MACINTOSH) && OS_MACINTOSH	debugstr(outputMessage);#endif}static void Message_SecretWarning(TQ3Warning warning, char *message){	char	outputMessage[512];	Message_Concatenate("Warning", warning, message, outputMessage);#if defined(OS_MACINTOSH) && OS_MACINTOSH	debugstr(outputMessage);#endif}#endif /* SECRET_ERROR */