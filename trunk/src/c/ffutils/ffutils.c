#include <ffutils.h>
#include <fcntl.h>	/* open64() and O_* flags */

#ifdef DONT_PUT_THIS_IN
struct field_index findfields( char *header, const char *delim ){
	int i;		/* field index */
	size_t dlen;	/* delimiter length */
	char *fs, *fe;	/* field start & end pointers */

	struct field_index f = { -1, -1, -1, -1, -1, -1 };

	dlen = strlen(delim);
	fs = header;
	i = 0;

	while( fs != NULL ){
		fe = strstr(fs, delim);
		if( fe == NULL )	/* last field */
			fe = header + strlen(header);

		/* printf("field %d: %.*s\n", i, fe - fs,fs); */

		if ( strncmp(fs, DT_LABEL_TIME, fe - fs) == 0 )
			f.time = i;
		else if ( strncmp(fs, DT_LABEL_USER, fe - fs) == 0 )
			f.userid = i;
		else if ( strncmp(fs, DT_LABEL_TYPE, fe - fs) == 0 )
			f.type = i;
		else if ( strncmp(fs, DT_LABEL_CAT, fe - fs) == 0 )
			f.cat = i;
		else if ( strncmp(fs, DT_LABEL_QTY, fe - fs) == 0 )
			f.quantity = i;
		else if ( strncmp(fs, DT_LABEL_REV, fe - fs) == 0 )
			f.revenue = i;

		/* don't want to assign fs = fe in case this is the last
		   field in the line & fe has been pointed at the end of
		   the string.
		 */
		fs = strstr(fs, delim);
		if( fs != NULL )
			fs += dlen;
		i++;
	}

	return f;
}
#endif

size_t fields_in_line(const char *l, const char *d){
	char *p = (char *) l;
	size_t f = 1;
	size_t dl;

	if ( l == NULL || d == NULL )
		return 0;

	dl = strlen(d);
	while ( (p = strstr(p, d)) != NULL ){
		f++;
		p += dl;
	}
	return f;
}

int get_line_field(char *cs, const char *ct, const size_t n, const int i, const char *delim) {
	int cti, cn;
	char *fstart, *fend;
	
	size_t dlen;

	if ( ! (delim && delim[0]) ) {
		strncpy( cs, ct, n );
		cs[n] = 0;
		return (strlen(cs));
	}

	dlen = strlen(delim);
	fstart = (char *) ct;
	for(cti=0; cti<i; cti++){
		fstart = strstr(fstart, delim);
		if( fstart == NULL ){
			cs[0] = 0x00;
			return -1;
		}
		fstart += dlen;
	}

	fend = strstr(fstart, delim);
	if( fend == NULL )
		fend = (char *)ct + strlen(ct);
	cn = (fend - fstart > n-1 ? n-1 : fend - fstart);
	strncpy( cs, fstart, cn );
	cs[cn] = 0x00;
	return cn;
}

char * field_start(const char *cs, size_t fn, const char *delim) {
	char *p;
	size_t dl;	/* delimiter length */
	int i;

	if ( fn > fields_in_line(cs, delim) )
		return NULL;

	dl = strlen(delim);
	p = (char *) cs;

	for ( i = 1; i < fn; i++ ) {
		p = strstr(p, delim);
		p += dl;
	}
	return p;
}

int mdyhms_datecmp(const char *a, const char *b){
	int ay, am, ad, ah, ai, as;
	int by, bm, bd, bh, bi, bs;
	sscanf(a, "%2d%*c%2d%*c%4d%*c%2d:%2d:%2d", &am, &ad, &ay, &ah, &ai, &as);
	sscanf(b, "%2d%*c%2d%*c%4d%*c%2d:%2d:%2d", &bm, &bd, &by, &bh, &bi, &bs);

	if( ay == by && am == bm && ad == bd &&
	    ah == bh && ai == bi && as == bs )
		return 0;
	if( ay < by )
		return -1;
	else if( ay == by ) {
		if( am < bm )
			return -1;
		else if( am == bm ) {
			if( ad < bd )
				return -1;
			else if( ad == bd ){
				if( ah < bh )
					return -1;
				else if( ah == bh ){
					if( ai < bi )
						return -1;
					else if( ai == bi ){
						if( as < bs )
							return -1;
					}
				}
			}
		}
	}
	return 1;
}

void chomp(char *s) {
	int l = strlen(s) - 1;
	while( l >= 0 && (s[l] == '\n' || s[l] == '\r') )
		s[l--] = '\0';
}

/* get's the next file specified in the trailing commandline args */
FILE * nextfile( int argc, char *argv[], int *optind, const char *mode ) {
	FILE *fp = NULL;
	int fd_flags = 0;	/* file open flags */
	int fd_mode = 0775;	/* file permissions mode */
	int fd;

	if ( strchr(mode, '+') )
		fd_flags |= O_RDWR;
	else if ( strchr(mode, 'r') )
		fd_flags |= O_RDONLY;
	else if ( strchr(mode, 'w') )
		fd_flags |= O_WRONLY | O_CREAT;
	else  if ( strchr(mode, 'a') )
		fd_flags |= O_WRONLY | O_CREAT | O_APPEND;

	fd_flags |= O_LARGEFILE;

	while ( *optind < argc ) {

		if ( (fd = open64( argv[(*optind)++], fd_flags )) != -1 ) {
			fp = fdopen( fd, mode );
			break;
		}

		perror( argv[ *optind - 1 ] );

	}

	return fp;
}


void expand_chars(char *s){
	char *w,	/* a working copy of the string */
	     *c, *p,	/* position markers w/in the string */
	     *e;	/* the expansion */

	if ( strchr(s, '\\') == NULL )
		return;

	w = malloc(strlen(s));
	memset(w, 0, strlen(s));

	p = s;
	while ( (c = strchr(p, '\\')) != NULL ) {
		strncat(w, p, c - p);
		switch (*(c+1)) {
			case 'a':	e = "\a"; break;
			case 'b':	e = "\b"; break;
			case 'f':	e = "\f"; break;
			case 'n':	e = "\n"; break;
			case 'r':	e = "\r"; break;
			case 't':	e = "\t"; break;
			case 'v':	e = "\v"; break;
			case '\\':	e = "\\"; break;
			default:	e = c +1; break;
		}
		strncat(w, e, 1);
		p = c = c + 2;
		
	}
	strcat(w, p);	/* copy everything after the last escape */
	strcpy(s, w);	/* put the working copy into the orignal */

	free(w);
}


#ifndef HAVE_GETLINE
ssize_t getline ( char **lineptr, size_t *n, FILE *stream ){

	/* initialize if it hasn't already been done -
	   initial buffer size == 128 */
	if( *n == 0 || *lineptr == NULL ){
		if( ( *lineptr = malloc(sizeof(char) << 7) ) == NULL ){
			return -1;
		}
		*n = sizeof(char) << 7;
	}

	if( fgets( *lineptr, *n, stream ) == NULL ){
		return 0;
	}
	while( (*lineptr)[ strlen(*lineptr)-1 ] != '\n' ){
		char buf[GETLINE_BUF_INC];
		if( ( *lineptr = realloc( *lineptr, *n + GETLINE_BUF_INC ) ) == NULL )
			return -1;
		*n += GETLINE_BUF_INC;
		fgets( buf, GETLINE_BUF_INC, stream );
		strcat( *lineptr, buf );
	}
	return strlen(*lineptr);
}
#endif

