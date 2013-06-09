//Author:SinSoul
//E-mail:nh6080@gmail.com
//Function:Used to export png and bmp images from unknown file
#include "windows.h"
#include "stdio.h"
#define FILE_MAP_START 0x0
int pngfinder(BYTE *cMapBuffer,DWORD dwViewSize,char *szDir)
{
	int sum;
	DWORD position,address,start,dBytesWritten;
	BOOL done_flag=FALSE;
	TCHAR szPngDir[255];
	sprintf(szPngDir,"%s\\PNG\\",szDir);
	CreateDirectory(szPngDir, NULL);
	for(sum=0,position=0; position<dwViewSize; position++)
	{	
		if(cMapBuffer[position]==0x89 &&cMapBuffer[position+1]==0x50 &&cMapBuffer[position+2]==0x4e &&cMapBuffer[position+3]==0x47
			&&cMapBuffer[position+4]==0x0d &&cMapBuffer[position+5]==0x0a &&cMapBuffer[position+6]==0x1a &&cMapBuffer[position+7]==0x0a)
		{
			address=(int)cMapBuffer+position;
			start=position;
			for(;!done_flag;position++)
			{
				//Find png end flag
				if(cMapBuffer[position]==0x49 &&cMapBuffer[position+1]==0x45 &&cMapBuffer[position+2]==0x4e &&cMapBuffer[position+3]==0x44
					&&cMapBuffer[position+4]==0xAE &&cMapBuffer[position+5]==0x42 &&cMapBuffer[position+6]==0x60 &&cMapBuffer[position+7]==0x82
					)
				{
					char filename[255];
					sum++;
					sprintf(filename,"%s%.3d.png",szPngDir,sum);
					HANDLE hPNGFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
					int end=(int)cMapBuffer+position+8;
					WriteFile (hPNGFile,&cMapBuffer[start],end-address,&dBytesWritten,NULL);
					CloseHandle(hPNGFile);
					printf("%.3d PNG Source Start Address:0x%.8x,END Address:0x%.8x,Size:0x%.8x.\n",sum,address-0x00400000,end-0x00400000,end-address);
					done_flag=TRUE;
				}
			}
			done_flag=FALSE;
		}
	}
	if(sum==0)
		RemoveDirectory(szPngDir);
	return sum;
}

int bmpfinder(BYTE *cMapBuffer,DWORD dwViewSize,char *szDir)
{
	int sum;
	DWORD position,address,dwBMPSize,dBytesWritten;
	TCHAR szBmpDir[255];
	sprintf(szBmpDir,"%s\\BMP\\",szDir);
	CreateDirectory(szBmpDir, NULL);
	for(sum=0,position=0; position<dwViewSize; position++)
	{			
		//Find BMP Head and Get BMP File Size
		if(cMapBuffer[position]==0x42 &&cMapBuffer[position+1]==0x4d&&cMapBuffer[position+6]==0x00)
		{
			address=(int)cMapBuffer+position;
			char filename[255];
			dwBMPSize=cMapBuffer[position+2]+cMapBuffer[position+3]*0x100+cMapBuffer[position+4]*0x10000+cMapBuffer[position+5]*0x1000000;
			//check BMP end flag,file size and Pointer
			//printf("Farthest Pointer:0x%.8x.\nCurrent Pointer:0x%.8x\n",&cMapBuffer[dwViewSize],&cMapBuffer[position+dwBMPSize-1]);
			if( (&cMapBuffer[dwViewSize] < &cMapBuffer[position+dwBMPSize-1]) || dwBMPSize>dwViewSize || cMapBuffer[position+dwBMPSize-1]!=0x00)
			{
				continue;
			}
			sum++;
			sprintf(filename,"%s%.3d.bmp",szBmpDir,sum);
			HANDLE hBMPFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			WriteFile (hBMPFile,&cMapBuffer[position],dwBMPSize,&dBytesWritten,NULL);
			CloseHandle(hBMPFile);
			printf("%.3d BMP Source Start Address:0x%.8x,END Address:0x%.8x,Size:0x%.8x.\n",sum,address-0x00400000,address+dwBMPSize-0x00400000,dwBMPSize);
		}
	}
	if(sum==0)
		RemoveDirectory(szBmpDir);
	return sum;
}


int wavfinder(BYTE *cMapBuffer,DWORD dwViewSize,char *szDir)
{
	int sum;
	DWORD position,address,dwBMPSize,dBytesWritten;
	TCHAR szBmpDir[255];
	sprintf(szBmpDir,"%s\\WAV\\",szDir);
	CreateDirectory(szBmpDir, NULL);
	for(sum=0,position=0; position<dwViewSize; position++)
	{			
		//Find WAV Head and Get WAV File Size
		if(cMapBuffer[position]=='R' 
			&&cMapBuffer[position+1]=='I'
			&&cMapBuffer[position+2]=='F'
			&&cMapBuffer[position+3]=='F'
			&&cMapBuffer[position+8]=='W'
			&&cMapBuffer[position+9]=='A'
			&&cMapBuffer[position+10]=='V'
			&&cMapBuffer[position+11]=='E'
			)
		{
			address=(int)cMapBuffer+position;
			char filename[255];
			dwBMPSize=cMapBuffer[position+4]+cMapBuffer[position+5]*0x100+cMapBuffer[position+6]*0x10000+cMapBuffer[position+7]*0x1000000;
			if (dwBMPSize==0)
			{
				continue;
			}
			sum++;
			sprintf(filename,"%s%.3d.wav",szBmpDir,sum);
			HANDLE hBMPFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			WriteFile (hBMPFile,&cMapBuffer[position],dwBMPSize,&dBytesWritten,NULL);
			CloseHandle(hBMPFile);
			printf("%.3d WAV Source Start Address:0x%.8x,END Address:0x%.8x,Size:0x%.8x.\n",sum,address-0x00400000,address+dwBMPSize-0x00400000,dwBMPSize);
		}

	}
	if(sum==0)
		RemoveDirectory(szBmpDir);
	return sum;
}


int main(int argc,char **argv)
{
	HANDLE hMapFile,hFile;
	DWORD dwFileSize,dwFileMapSize,dwMapViewSize,dwFileMapStart,dwSysGran;
	SYSTEM_INFO SysInfo;
	BYTE *lpMapBuffer;
	BOOL done_flag=FALSE;
	int sum_png,sum_bmp,sum_wav;
	TCHAR szDir[255],szDrive[255],szFile[255],szDirectory[255];
	if(argc<=1)
	{
		printf("Please select a file and drag it on my icon.\n");
		getchar();
		return 1;
	}
	_splitpath(argv[0],szDrive,szDir,NULL,NULL);
	_splitpath(argv[1],NULL,NULL,szFile,NULL);
	hFile = CreateFile(argv[1],GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Can't find or open File:%s\n",argv[1]);
		Sleep(3000);
		return 1;
	}
	dwFileSize = GetFileSize(hFile,NULL);
	printf("File Name:%s\nFile Size:0x%.8x\n",argv[1],dwFileSize);
	
	GetSystemInfo(&SysInfo);
	dwSysGran = SysInfo.dwAllocationGranularity;
	dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;
	dwMapViewSize = (FILE_MAP_START % dwSysGran) + dwFileSize;
	dwFileMapSize = FILE_MAP_START + dwFileSize;

	hMapFile = CreateFileMapping( hFile,NULL,PAGE_READWRITE,0,dwFileMapSize,NULL);
	if (hMapFile == NULL) 
	{
		printf("CreateFileMapping Error: %d\n", GetLastError());
		Sleep(3000);
		return 1;
	}

	lpMapBuffer = (BYTE *)MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS,0,dwFileMapStart,dwMapViewSize);
	if (lpMapBuffer == NULL) 
	{
		printf("MapViewOfFile Error: %d\n", GetLastError());
		Sleep(3000);
		return 1;
	}

	printf ("Map View Start:0x%.8x\nMap View Size :0x%.8x\n",dwFileMapStart,dwMapViewSize);
	sprintf(szDirectory,"%s%s%s",szDrive,szDir,szFile);
	CreateDirectory(szDirectory, NULL);

	sum_png=pngfinder(lpMapBuffer,dwFileSize,szDirectory);
	sum_bmp=bmpfinder(lpMapBuffer,dwFileSize,szDirectory);
	sum_wav=wavfinder(lpMapBuffer,dwFileSize,szDirectory);
	printf("Total:\n\t %d PNG Sources.\n\t %d BMP Sources.\n\t %d WAV Sources. ",sum_png,sum_bmp,sum_wav);
	
	CloseHandle(hFile);
	CloseHandle(hMapFile);
	if(sum_png==0&&sum_bmp==0)
		RemoveDirectory(szDirectory);
	getchar();
	return 0;
}




