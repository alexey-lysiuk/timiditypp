/*

    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   common.c

   */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <ctype.h>
#ifndef __W32__
#include <unistd.h>
#else
#include <process.h>
#include <io.h>
#endif /* __W32__ */
#include "timidity.h"
#include "common.h"
#include "output.h"
#include "controls.h"
#include "arc.h"
#include "nkflib.h"
#include "wrd.h"
#include "strtab.h"

/* RAND_MAX must defined in stdlib.h
 * Why RAND_MAX is not defined at SunOS?
 */
#if defined(sun) && !defined(SOLARIS) && !defined(RAND_MAX)
#define RAND_MAX ((1<<15)-1)
#endif

/* #define MIME_CONVERSION */

char *program_name, current_filename[1024];
MBlockList tmpbuffer;
ArchiveFileList *archive_file_list = NULL;
char *output_text_code = NULL;

#ifdef DEFAULT_PATH
    /* The paths in this list will be tried whenever we're reading a file */
    static PathList defaultpathlist={DEFAULT_PATH,0};
    static PathList *pathlist=&defaultpathlist; /* This is a linked list */
#else
    static PathList *pathlist=0;
#endif

const char *note_name[] =
{
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

static int url_copyfile(URL url, char *path)
{
    FILE *fp;
    char buff[BUFSIZ];
    int n;

    if((fp = fopen(path, "wb")) == NULL)
	return -1;
    while((n = url_read(url, buff, sizeof(buff))) > 0)
	fwrite(buff, 1, n, fp);
    fclose(fp);
    return 0;
}

static char *make_temp_filename(char *ext)
{
    static char buff[1024], *tmpdir;
    static int cnt;

    if(ext == NULL)
	ext = "";

#ifdef TMPDIR
    tmpdir = TMPDIR;
#else
    tmpdir = getenv("TMPDIR");
#endif
    if(tmpdir == NULL || strlen(tmpdir) == 0)
	tmpdir = PATH_STRING "tmp" PATH_STRING;
    if(tmpdir[strlen(tmpdir) - 1] == PATH_SEP)
	sprintf(buff, "%stimidity-tmp%d-%d%s",
		tmpdir, cnt++, (int)getpid(), ext);
    else
	sprintf(buff, "%s" PATH_STRING "timidity-tmp%d-%d%s",
		tmpdir, cnt++, (int)getpid(), ext);
    return buff;
}

/* Try to open a file for reading. If the filename ends in one of the
   defined compressor extensions, pipe the file through the decompressor */
struct timidity_file *try_to_open(char *name, int decompress)
{
    struct timidity_file *tf;
    URL url;
    int len;

    archive_file_list = add_archive_list(archive_file_list, name);
    errno = 0;
    url = archive_file_extract_open(archive_file_list, name);
    if(url == NULL)
    {
	if(last_archive_file_list != NULL &&
	   last_archive_file_list->errstatus != 0)
	{
	    errno = last_archive_file_list->errstatus;
	    return NULL;
	}

	if(strchr(name, '#') != NULL)
	    return NULL;

	if((url = url_open(name)) == NULL)
	    return NULL;
    }

    tf = (struct timidity_file *)safe_malloc(sizeof(struct timidity_file));
    tf->url = url;
    tf->tmpname = NULL;

    len = strlen(name);
    if(decompress && len >= 3 && strcasecmp(name + len - 3, ".gz") == 0)
    {
	int method;

	if(!IS_URL_SEEK_SAFE(tf->url))
	{
	    if((tf->url = url_cache_open(tf->url, 1)) == NULL)
	    {
		close_file(tf);
		return NULL;
	    }
	}

	method = skip_gzip_header(tf->url);
	if(method == ARCHIVEC_DEFLATED)
	{
	    url_cache_disable(tf->url);
	    if((tf->url = url_inflate_open(tf->url, -1, 1)) == NULL)
	    {
		close_file(tf);
		return NULL;
	    }

	    /* success */
	    return tf;
	}
	/* fail */
	url_rewind(tf->url);
	url_cache_disable(tf->url);
    }

#if defined(DECOMPRESSOR_LIST)
    if(decompress)
    {
	static char *decompressor_list[] = DECOMPRESSOR_LIST, **dec;
	char tmp[1024], *tmpname;

	/* Check if it's a compressed file */
	for(dec = decompressor_list; *dec; dec += 2)
	{
	    if(!check_file_extension(name, *dec, 0))
		continue;
	    tmpname = make_temp_filename(*dec);
	    unlink(tmpname); /* For security */
	    tf->tmpname = safe_strdup(tmpname);
	    if(url_copyfile(tf->url, tmpname) == -1)
	    {
		close_file(tf);
		return NULL;
	    }
	    url_close(tf->url);
	    sprintf(tmp, *(dec+1), tf->tmpname);
	    if((tf->url = url_pipe_open(tmp)) == NULL)
	    {
		close_file(tf);
		return NULL;
	    }

	    break;
	}
    }
#endif /* DECOMPRESSOR_LIST */

#if defined(PATCH_CONVERTERS)
    if(decompress == 2)
    {
	static char *decompressor_list[] = PATCH_CONVERTERS, **dec;
	char tmp[1024], *tmpname;

	/* Check if it's a compressed file */
	for(dec = decompressor_list; *dec; dec += 2)
	{
	    if(!check_file_extension(name, *dec, 0))
		continue;
	    tmpname = make_temp_filename(*dec);
	    unlink(tmpname); /* For security */
	    tf->tmpname = safe_strdup(tmpname);
	    if(url_copyfile(tf->url, tmpname) == -1)
	    {
		close_file(tf);
		return NULL;
	    }
	    url_close(tf->url);
	    sprintf(tmp, *(dec+1), tf->tmpname);
	    if((tf->url = url_pipe_open(tmp)) == NULL)
	    {
		close_file(tf);
		return NULL;
	    }

	    break;
	}
    }
#endif /* PATCH_CONVERTERS */
    
    return tf;
}

static int is_url_prefix(char *name)
{
    int i;

    static char *url_proto_names[] =
    {
	"file:",
#ifdef SUPPORT_SOCKET
	"http://",
	"ftp://",
	"news://",
#endif /* SUPPORT_SOCKET */
	"mime:",
	NULL
    };
    for(i = 0; url_proto_names[i]; i++)
	if(strncmp(name, url_proto_names[i], strlen(url_proto_names[i])) == 0)
	    return 1;

#ifdef __W32__
    /* [A-Za-z]: (for Windows) */
    if((('A' <= name[0] && name[0] <= 'Z') ||
	('a' <= name[0] && name[0] <= 'z')) &&
       name[1] == ':')
	return 1;
#endif /* __W32__ */

    return 0;
}

struct timidity_file *open_with_mem(char *mem, int32 memlen, int noise_mode)
{
    URL url;
    struct timidity_file *tf;

    errno = 0;
    if((url = url_mem_open(mem, memlen, 0)) == NULL)
    {
	if(noise_mode >= 2)
	    ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "Can't open.");
	return NULL;
    }
    tf = (struct timidity_file *)safe_malloc(sizeof(struct timidity_file));
    tf->url = url;
    tf->tmpname = NULL;
    return tf;
}

/* This is meant to find and open files for reading, possibly piping
   them through a decompressor. */
struct timidity_file *open_file(char *name, int decompress, int noise_mode)
{
  struct timidity_file *tf;
  PathList *plp=pathlist;
  int l;

  if (!name || !(*name))
    {
      if(noise_mode)
        ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "Attempted to open nameless file.");
      return 0;
    }

  /* First try the given name */
  strncpy(current_filename, url_unexpand_home_dir(name), 1023);
  current_filename[1023]='\0';

  if(noise_mode)
    ctl->cmsg(CMSG_INFO, VERB_DEBUG, "Trying to open %s", current_filename);
  if ((tf=try_to_open(current_filename, decompress)))
    return tf;

#ifdef __MACOS__
  if(errno)
#else
  if(errno && errno != ENOENT)
#endif
    {
	if(noise_mode)
	    ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%s: %s",
		      current_filename, strerror(errno));
	return 0;
    }

  if (name[0] != PATH_SEP && !is_url_prefix(name))
    while (plp)  /* Try along the path then */
      {
	*current_filename=0;
	l=strlen(plp->path);
	if(l)
	  {
	    strcpy(current_filename, plp->path);
	    if(current_filename[l-1] != PATH_SEP &&
	       current_filename[l-1] != '#' &&
	       name[0] != '#')
		strcat(current_filename, PATH_STRING);
	  }
	strcat(current_filename, name);
	if(noise_mode)
	    ctl->cmsg(CMSG_INFO, VERB_DEBUG,
		      "Trying to open %s", current_filename);
	if ((tf=try_to_open(current_filename, decompress)))
	  return tf;
#ifdef __MACOS__
	if(errno)
#else
	if(errno && errno != ENOENT)
#endif
	{
	    if(noise_mode)
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%s: %s",
			  current_filename, strerror(errno));
	    return 0;
	  }
	plp=plp->next;
      }

  /* Nothing could be opened. */

  *current_filename=0;

  if (noise_mode>=2)
      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%s: %s", name,
		errno ? strerror(errno) : "Can't open file");

  return 0;
}

/* This closes files opened with open_file */
void close_file(struct timidity_file *tf)
{
    int save_errno = errno;
    if(tf->url != NULL)
    {
#ifndef __W32__
	if(tf->tmpname != NULL)
	{
	    int i;
	    /* dispose the pipe garbage */
	    for(i = 0; tf_getc(tf) != EOF && i < 0xFFFF; i++)
		;
	}
#endif /* __W32__ */
	url_close(tf->url);
    }
    if(tf->tmpname != NULL)
    {
	unlink(tf->tmpname); /* remove temporary file */
	free(tf->tmpname);
    }
    free(tf);
    errno = save_errno;
}

/* This is meant for skipping a few bytes. */
void skip(struct timidity_file *tf, size_t len)
{
    url_skip(tf->url, (long)len);
}

char *tf_gets(char *buff, int n, struct timidity_file *tf)
{
    return url_gets(tf->url, buff, n);
}

long tf_read(void *buff, int32 size, int32 nitems, struct timidity_file *tf)
{
    return url_nread(tf->url, buff, size * nitems) / size;
}

long tf_seek(struct timidity_file *tf, long offset, int whence)
{
    long prevpos;

    prevpos = url_seek(tf->url, offset, whence);
    if(prevpos == -1)
	ctl->cmsg(CMSG_WARNING, VERB_NORMAL,
		  "Warning: Can't seek file position");
    return prevpos;
}

long tf_tell(struct timidity_file *tf)
{
    long pos;

    pos = url_tell(tf->url);
    if(pos == -1)
    {
	ctl->cmsg(CMSG_WARNING, VERB_NORMAL,
		  "Warning: Can't get current file position");
	return (long)tf->url->nread;
    }

    return pos;
}

void safe_exit(int status)
{
    if(play_mode->fd != -1)
    {
	play_mode->acntl(PM_REQ_DISCARD, NULL);
	play_mode->close_output();
    }
    ctl->close();
    wrdt->close();
    exit(status);
    /*NOTREACHED*/
}

/* This'll allocate memory or die. */
void *safe_malloc(size_t count)
{
    void *p;
    static int errflag = 0;

    if(errflag)
	safe_exit(10);
    if(count > MAX_SAFE_MALLOC_SIZE)
    {
	errflag = 1;
	ctl->cmsg(CMSG_FATAL, VERB_NORMAL,
		  "Strange, I feel like allocating %d bytes. "
		  "This must be a bug.", count);
    }
    else if((p = (void *)malloc(count)) != NULL)
	return p;
    else
    {
	if(count == 0) {
	    /* Some malloc routine return NULL if count is zero, such as
	     * malloc routine from libmalloc.a of Solaris.
	     * But TiMidity doesn't want to return NULL even if count is zero.
	     */
	    return safe_malloc(1);
	}

	errflag = 1;
	ctl->cmsg(CMSG_FATAL, VERB_NORMAL,
		  "Sorry. Couldn't malloc %d bytes.", count);
    }
#ifdef ABORT_AT_FATAL
    abort();
#endif /* ABORT_AT_FATAL */
    safe_exit(10);
    /*NOTREACHED*/
}

void *safe_large_malloc(size_t count)
{
    void *p;
    static int errflag = 0;

    if(errflag)
	safe_exit(10);
    if((p = (void *)malloc(count)) != NULL)
	return p;
    errflag = 1;
    ctl->cmsg(CMSG_FATAL, VERB_NORMAL,
	      "Sorry. Couldn't malloc %d bytes.", count);
#ifdef ABORT_AT_FATAL
    abort();
#endif /* ABORT_AT_FATAL */
    safe_exit(10);
    /*NOTREACHED*/
}

void *safe_realloc(void *ptr, size_t count)
{
    void *p;
    static int errflag = 0;

    if(errflag)
	safe_exit(10);
    if(count > MAX_SAFE_MALLOC_SIZE)
    {
	errflag = 1;
	ctl->cmsg(CMSG_FATAL, VERB_NORMAL,
		  "Strange, I feel like allocating %d bytes. "
		  "This must be a bug.", count);
    }
    else if((p = (void *)realloc(ptr, count)) != NULL)
	return p;
    else
    {
	errflag = 1;
	ctl->cmsg(CMSG_FATAL, VERB_NORMAL,
		  "Sorry. Couldn't realloc %d bytes.", count);
    }
#ifdef ABORT_AT_FATAL
    abort();
#endif /* ABORT_AT_FATAL */
    safe_exit(10);
    /*NOTREACHED*/
}

/* This'll allocate memory or die. */
char *safe_strdup(char *s)
{
    char *p;
    static int errflag = 0;

    if(errflag)
	safe_exit(10);

    if(s == NULL)
	p = strdup("");
    else
	p = strdup(s);
    if(p != NULL)
	return p;
    errflag = 1;
    ctl->cmsg(CMSG_FATAL, VERB_NORMAL, "Sorry. Couldn't alloc memory.");
#ifdef ABORT_AT_FATAL
    abort();
#endif /* ABORT_AT_FATAL */
    safe_exit(10);
    /*NOTREACHED*/
}

/* This adds a directory to the path list */
void add_to_pathlist(char *s)
{
  PathList *plp=safe_malloc(sizeof(PathList));
  strcpy((plp->path=safe_malloc(strlen(s)+1)),s);
  plp->next=pathlist;
  pathlist=plp;
}

#ifndef HAVE_VOLATILE
/*ARGSUSED*/
int volatile_touch(void *dmy) {return 1;}
#endif /* HAVE_VOLATILE */

/* code converters */
static void code_convert_dump(char *in, char *out, int maxlen, char *ocode)
{
    if(ocode == NULL)
	ocode = output_text_code;

    if(ocode != NULL && ocode != (char *)-1
	&& (strstr(ocode, "ASCII") || strstr(ocode, "ascii")))
    {
	int i;

	if(out == NULL)
	    out = in;
	for(i = 0; i < maxlen && in[i]; i++)
	    if(in[i] < ' ' || in[i] >= 127)
		out[i] = '.';
	    else
		out[i] = in[i];
	out[i]='\0';
    }
    else /* "NOCNV" */
    {
	if(out == NULL)
	    return;
	strncpy(out, in, maxlen);
	out[maxlen] = '\0';
    }
}

#ifdef JAPANESE
static void code_convert_japan(char *in, char *out, int maxlen,
			       char *icode, char *ocode)
{
    static char *mode = NULL, *wrd_mode = NULL;

    if(ocode != NULL && ocode != (char *)-1)
    {
	nkf_convert(in, out, maxlen, icode, ocode);
	if(out != NULL)
	    out[maxlen] = '\0';
	return;
    }

    if(mode == NULL || wrd_mode == NULL)
    {
	mode = output_text_code;
	if(mode == NULL || strstr(mode, "AUTO"))
	{
#ifndef __W32__
	    mode = getenv("LANG");
#else
	    mode = "SJIS";
	    wrd_mode = "SJISK";
#endif
	    if(mode == NULL || *mode == '\0')
	    {
		mode = "ASCII";
		wrd_mode = mode;
	    }
	}

	if(strstr(mode, "ASCII") ||
	   strstr(mode, "ascii"))
	{
	    mode = "ASCII";
	    wrd_mode = mode;
	}
	else if(strstr(mode, "NOCNV") ||
		strstr(mode, "nocnv"))
	{
	    mode = "NOCNV";
	    wrd_mode = mode;
	}
#ifndef	HPUX
	else if(strstr(mode, "EUC") ||
		strstr(mode, "euc") ||
		strstr(mode, "ujis") ||
		strcmp(mode, "japanese") == 0)
	{
	    mode = "EUC";
	    wrd_mode = "EUCK";
	}
	else if(strstr(mode, "SJIS") ||
		strstr(mode, "sjis"))
	{
	    mode = "SJIS";
	    wrd_mode = "SJISK";
	}
#else
	else if(strstr(mode, "EUC") ||
		strstr(mode, "euc") ||
		strstr(mode, "ujis"))
	{
	    mode = "EUC";
	    wrd_mode = "EUCK";
	}
	else if(strstr(mode, "SJIS") ||
		strstr(mode, "sjis") ||
		strcmp(mode, "japanese") == 0)
	{
	    mode = "SJIS";
	    wrd_mode = "SJISK";
	}
#endif	/* HPUX */
	else if(strstr(mode,"JISk")||
		strstr(mode,"jisk"))
	{
	    mode = "JISK";
	    wrd_mode = mode;
	}
	else if(strstr(mode, "JIS") ||
		strstr(mode, "jis"))
	{
	    mode = "JIS";
	    wrd_mode = "JISK";
	}
	else if(strcmp(mode, "ja") == 0)
	{
	    mode = "EUC";
	    wrd_mode = "EUCK";
	}
	else
	{
	    mode = "NOCNV";
	    wrd_mode = mode;
	}
    }

    if(ocode == NULL)
    {
	if(strcmp(mode, "NOCNV") == 0)
	{
	    if(out == NULL)
		return;
	    strncpy(out, in, maxlen);
	    out[maxlen] = '\0';
	}
	else if(strcmp(mode, "ASCII") == 0)
	    code_convert_dump(in, out, maxlen, "ASCII");
	else
	{
	    nkf_convert(in, out, maxlen, icode, mode);
	    if(out != NULL)
		out[maxlen] = '\0';
	}
    }
    else if(ocode == (char *)-1)
    {
	if(strcmp(wrd_mode, "NOCNV") == 0)
	{
	    if(out == NULL)
		return;
	    strncpy(out, in, maxlen);
	    out[maxlen] = '\0';
	}
	else if(strcmp(wrd_mode, "ASCII") == 0)
	    code_convert_dump(in, out, maxlen, "ASCII");
	else
	{
	    nkf_convert(in, out, maxlen, icode, wrd_mode);
	    if(out != NULL)
		out[maxlen] = '\0';
	}
    }
}
#endif /* JAPANESE */

void code_convert(char *in, char *out, int outsiz, char *icode, char *ocode)
{
#if !defined(MIME_CONVERSION) && defined(JAPANESE)
    int i;
    /* check ASCII string */
    for(i = 0; in[i]; i++)
	if(in[i] < ' ' || in[i] >= 127)
	    break;
    if(!in[i])
    {
	if(out == NULL)
	    return;
	strncpy(out, in, outsiz - 1);
	out[outsiz - 1] = '\0';
	return; /* All ASCII string */
    }
#endif /* MIME_CONVERSION */

    if(ocode != NULL && ocode != (char *)-1)
    {
	if(strcasecmp(ocode, "nocnv") == 0)
	{
	    if(out == NULL)
		return;
	    outsiz--;
	    strncpy(out, in, outsiz);
	    out[outsiz] = '\0';
	    return;
	}

	if(strcasecmp(ocode, "ascii") == 0)
	{
	    code_convert_dump(in, out, outsiz - 1, "ASCII");
	    return;
	}
    }

#if defined(JAPANESE)
    code_convert_japan(in, out, outsiz - 1, icode, ocode);
#else
    code_convert_dump(in, out, outsiz - 1, ocode);
#endif
}

/* EAW -- insert stuff from playlist files
 *
 * Tue Apr 6 1999: Modified by Masanao Izumo <mo@goice.co.jp>
 *                 One pass implemented.
 */
static char **expand_file_lists(char **files, int *nfiles_in_out)
{
    int nfiles;
    int i;
    char input_line[256];
    char *pfile;
    static const char *testext = ".m3u.pls.asx.M3U.PLS.ASX";
    struct timidity_file *list_file;
    char *one_file[1];
    int one;

    /* Recusive global */
    static StringTable st;
    static int error_outflag = 0;
    static int depth = 0;

    if(depth >= 16)
    {
	if(!error_outflag)
	{
	    ctl->cmsg(CMSG_ERROR, VERB_NORMAL,
		      "Probable loop in playlist files");
	    error_outflag = 1;
	}
	return NULL;
    }

    if(depth == 0)
    {
	error_outflag = 0;
	init_string_table(&st);
    }
    nfiles = *nfiles_in_out;

    /* count number of new files to add */
    for(i = 0; i < nfiles; i++)
    {
	/* extract the file extension */
	pfile = strrchr(files[i], '.');

	if(*files[i] == '@' || (pfile != NULL && strstr(testext, pfile)))
	{
	    /* Playlist file */
            if(*files[i] == '@')
		list_file = open_file(files[i] + 1, 1, 1);
            else
		list_file = open_file(files[i], 1, 1);
            if(list_file)
	    {
                while(tf_gets(input_line, sizeof(input_line), list_file)
		      != NULL) {
            	    if(*input_line == '\n' || *input_line == '\r')
			continue;
		    if((pfile = strchr(input_line, '\r')))
		    	*pfile = '\0';
		    if((pfile = strchr(input_line, '\n')))
		    	*pfile = '\0';
		    one_file[0] = input_line;
		    one = 1;
		    depth++;
		    expand_file_lists(one_file, &one);
		    depth--;
		}
                close_file(list_file);
            }
        }
	else /* Other file */
	    put_string_table(&st, files[i], strlen(files[i]));
    }

    if(depth)
	return NULL;
    *nfiles_in_out = st.nstring;
    return make_string_array(&st);
}

char **expand_file_archives(char **files, int *nfiles_in_out)
{
    int nfiles;

    nfiles = *nfiles_in_out;
    files = expand_file_lists(files, &nfiles);

    archive_file_list = make_archive_list(archive_file_list, nfiles, files);
    if(archive_file_list != NULL)
    {
	char **new_files;
	int    new_nfiles;

	new_nfiles = nfiles;
	new_files = expand_archive_names(archive_file_list,
					 &new_nfiles, files);
	if(new_files != NULL)
	{
	    free(files[0]);
	    free(files);
	    nfiles = new_nfiles;
	    files  = new_files;
	}
    }
    *nfiles_in_out = nfiles;
    return files;
}

#ifdef RAND_MAX
int int_rand(int n)
{
    if(n < 0)
    {
	if(n == -1)
	    srand(time(NULL));
	else
	    srand(-n);
	return n;
    }
    return (int)(n * (double)rand() * (1.0 / (RAND_MAX + 1.0)));
}
#else
int int_rand(int n)
{
    static unsigned long rnd_seed = 0xabcd0123;

    if(n < 0)
    {
	if(n == -1)
	    rnd_seed = time(NULL);
	else
	    rnd_seed = -n;
	return n;
    }

    rnd_seed *= 69069UL;
    return (int)(n * (double)(rnd_seed & 0xffffffff) *
		 (1.0 / (0xffffffff + 1.0)));
}
#endif /* RAND_MAX */

int check_file_extension(char *filename, char *ext, int decompress)
{
    int len, elen, i;
#if defined(DECOMPRESSOR_LIST)
    char *dlist[] = DECOMPRESSOR_LIST;
#endif /* DECOMPRESSOR_LIST */

    len = strlen(filename);
    elen = strlen(ext);
    if(len > elen && strncasecmp(filename + len - elen, ext, elen) == 0)
	return 1;

    if(decompress)
    {
	/* Check gzip'ed file name */

	if(len > 3 + elen &&
	   strncasecmp(filename + len - elen - 3 , ext, elen) == 0 &&
	   strncasecmp(filename + len - 3, ".gz", 3) == 0)
	    return 1;

#if defined(DECOMPRESSOR_LIST)
	for(i = 0; dlist[i]; i += 2)
	{
	    int dlen;

	    dlen = strlen(dlist[i]);
	    if(len > dlen + elen &&
	       strncasecmp(filename + len - elen - dlen , ext, elen) == 0 &&
	       strncasecmp(filename + len - dlen, dlist[i], dlen) == 0)
		return 1;
	}
#endif /* DECOMPRESSOR_LIST */
    }
    return 0;
}

void randomize_string_list(char **strlist, int n)
{
    int i, j;
    char *tmp;
    for(i = 0; i < n; i++)
    {
	j = int_rand(n - i);
	tmp = strlist[j];
	strlist[j] = strlist[n - i - 1];
	strlist[n - i - 1] = tmp;
    }
}

int pathcasecmp(const char *p1, const char *p2)
{
    const unsigned char *s1, *s2;
    int c1, c2;

    s1 = *(const unsigned char **)p1;
    s2 = *(const unsigned char **)p2;
    while(*s1 && *s2)
    {
	c1 = tolower((int)*s1);
	c2 = tolower((int)*s2);
	if(c1 == PATH_SEP) c1 = 0;
	if(c2 == PATH_SEP) c2 = 0;
	if(c1 != c2)
	    return c1 - c2;
	s1++;
	s2++;
    }

    c1 = tolower((int)*s1);
    c2 = tolower((int)*s2);
    return c1 - c2;
}

void sort_pathname(char **files, int nfiles)
{
    qsort(files, nfiles, sizeof(char *),
	  (int (*)(const void *, const void *))pathcasecmp);
}
