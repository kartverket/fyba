///////////////////////////////////////////////////////////////////////////////////
// SPLITPATH.cpp
// Functions
//        UT_splitpath - split a full path name
//

#include <cstring>
#include <fyut.h>

void UT_splitFN(char *filename, char* name, char* ext) {
	char* lastDot = strrchr(filename, '.');
	if(lastDot != NULL) {
		UT_StrCopy(ext, lastDot, _MAX_EXT);
		if(strlen(filename) > _MAX_EXT) {
			name[_MAX_EXT-1] = '\0';
		}
		/* replace the posisiton of last dot in filename
        	with end-of-sting */
	    (*lastDot) = '\0'; 
	} else {
		(*ext) = '\0';
	}
	UT_StrCopy(name, filename, _MAX_FNAME);
	if(strlen(filename) > _MAX_FNAME) {
			name[_MAX_FNAME-1] = '\0';
	}
}

/*
CH UT_splitpath
CD ==============================================================
CD Splits a path into drive (X: for windows, "" for linux),
CD directory, filename (w/o extension) and extension.
CD
CD Parameters:
CD Type        Name             I/O  Explanation
CD -------------------------------------------------------------
CD const char *pathP             i  Path to split
CD char       *driveP            o  Drive-part of path
CD char       *dirP              o  Directory-part
CD char       *nameP             o  Filename w/o extension
CD char       *extP              o  extension
CD  ==============================================================
*/
SK_EntPnt_UT void UT_splitpath(const char *pathP, char *driveP, char *dirP, char *nameP, char *extP) {
	char local_path[PATH_MAX]; /* Copy of pathP i case we modify it */
	char tmp[PATH_MAX]; /* Copy of pathP i case we modify it */
	char filename[PATH_MAX];
	(*driveP) = (*dirP) = (*nameP) = (*extP) = '\0';

	strcpy(local_path, pathP);
	strcpy(tmp, local_path);
	/* Under linux, driveP is always \0 */
	#ifdef WIN32
		/* Afaik, there is only ONE : in windows filenames */
		char* theColon = strrchr(tmp, ':');
		if(theColon != NULL) {
			/* We overwrite local_path here, because after this the code
			is equal for win/lin if the drive-part is removed */
			UT_StrCopy(local_path, theColon+1, PATH_MAX);
			(*(theColon + 1)) = '\0'; // set a \0 after the color (inside tmp!)
			UT_StrCopy(driveP, tmp, _MAX_DRIVE);
			if (strlen(tmp) > _MAX_DRIVE) { // how would this even happen?
				driveP[_MAX_DRIVE-1] = '\0';
			}
		}
	#endif

	strcpy(tmp, local_path);
	
	char* lastSlash = strrchr(tmp, UT_SLASH);
	
	/* Set dirP */
	if(lastSlash != NULL) {
		/* +1 because we don't want the / in the filename */
		char filename[PATH_MAX]; /* UT_splitFN might modify filename */
		strcpy(filename,lastSlash+1);
		if (strcmp(filename, ".") != 0) {
			UT_splitFN(filename, nameP, extP);
		    (*(tmp + (lastSlash - tmp + 1))) = '\0';	
		}
		UT_StrCopy(dirP, tmp, _MAX_DIR);
		// No null-character is implicitly appended at the end of destination if source is longer than num.
		if (strlen(tmp) > _MAX_DIR) {
			dirP[_MAX_DIR-1] = '\0';
		}
	} else {
		if (strcmp(".", local_path) == 0) { /* Hard-coded to mimic old behaviour */
		 	strcpy(dirP, ".");
		} else {
			UT_splitFN(tmp, nameP, extP);
		}
	}
}

