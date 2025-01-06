#include <stdio.h>
#include <tchar.h>

#include "utils.h"


int _tmain(int argc, const TCHAR **argv)
{

	// FOLDERID_Downloads

	PWSTR strbuff = NULL;

	if (get_downloads_folder(&strbuff) == 0)
	{
        // MessageBoxW(NULL, strbuff, TEXT("Downloads dir current user"), MB_OK);
		;
        printf("%d\n", init_dirs(strbuff, FALSE));
	} 

    free_downloads_folder(&strbuff);
    

	return 0;
}
