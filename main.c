/* Language patcher 0.1*/
/* The Lemon Man [ITA] */

#include <pspkernel.h>
#include <systemctrl.h>

PSP_MODULE_INFO("langPatcher_TheLemonMan_0.1", 0x1000, 1, 0);

u32 k1;
static int languagePatch;

static int (*sceUtilityGetSystemParamInt)( int id, int *value ) = NULL;

int sceUtilityGetSystemParamInt_patched( int id, int *value )
{
	int ret;
	
	k1 = pspSdkGetK1();
	
	ret = sceUtilityGetSystemParamInt(id, value);
	if (ret != 0) goto noPatch_exit;
	
	if (id == 8)
	{
		*value = languagePatch;
	}

noPatch_exit:
	pspSdkSetK1(k1);
	
	return ret;
}

int module_start( SceSize args, void *argp )
{
	SceUID file = sceIoOpen("ms0:/seplugins/language.bin", PSP_O_RDONLY, 0777);
	if (!file)
	{
		return 0;
	}
	sceIoRead(file, &languagePatch, 1);
	sceIoClose(file);
	if ((languagePatch < 0) || (languagePatch > 11))
	{
		return 0;
	}
	
	u32 functionAddress = sctrlHENFindFunction("sceUtility_Driver", "sceUtility", 0xA5DA2406);
	sceUtilityGetSystemParamInt = (void *)functionAddress;
	
	sctrlHENPatchSyscall(functionAddress, sceUtilityGetSystemParamInt_patched);
	
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

	return 0;
}

int module_stop( SceSize args, void *argp )
{
	return 0;
}
