#include <ffutils.h>
#include <fcntl.h>	/* open64() and O_* flags */
#include <ctype.h>	/* isdigit(), isspace() */

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


/** @brief extract a field from a delimited string.
  *
  * copies at most <i>n</i> characters from field <i>i</i> of
  * <i>delim</i>-delimited string <i>ct</i> into <i>cs</i>.
  * 
  * @param cs destination buffer
  * @param ct delimited string
  * @param n max chars to copy into buffer
  * @param i field to be copied (0-based)
  * @param delim delimiter of ct
  * 
  * @return number of chars copied into buffer, or -1 if i is greater
  * than the number of fields in ct
  */
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

void trim(char *str){
	int i = strlen(str) - 1;
	for(; isspace(str[i]) && i > 0; i--)
		str[i] = '\0';
	return;
}

/* get's the next file specified in the trailing commandline args */
FILE * nextfile( int argc, char *argv[], int *optind, const char *mode ) {
	FILE *fp = NULL;
	int fd_flags = 0;	/* file open flags */
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


/* used in expand_nums() */
static size_t arr_resize(	void **array,
				const size_t dsize,
				const size_t oldsize,
				const size_t add
){
	if( realloc( *array, dsize * (oldsize + add) ) == NULL ){
		return 0;
	}
	return (oldsize + add);
}


ssize_t expand_nums ( char *arg, int **array, size_t *array_size ) {
	int i;
	char *token;

	if (arg == NULL || strlen(arg) == 0) {
		return 0;
	}

	/* check the string syntax */
	for ( i=0; arg[i] != '\0'; i++ ) {
		if( (! isdigit(arg[i]))
		    && arg[i] != ','
		    && arg[i] != '-' ) {
			return -2;
		}
	}
	i = 0;

	if (*array == NULL && *array_size == 0) {
		*array = malloc( sizeof(int) * FFUTILS_RESIZE_AMT );
		if ( *array == NULL ) {
			return -1;
		}
		*array_size = FFUTILS_RESIZE_AMT;
	}

	if( strchr(arg,',') == NULL && strchr(arg,'-') == NULL ){
		sscanf(arg,"%u",&((*array)[0]));
		return 1;
	}

	token = strtok( arg, "," );

	while( token != NULL ){

		if ( i >= *array_size ){
			if ( (*array_size = arr_resize((void**)array, sizeof(int),
					*array_size, FFUTILS_RESIZE_AMT)) == 0){
				return -1;
			}
		}

		if ( strchr(token, '-') == NULL ) {
			sscanf( token, "%u", &((*array)[i]) );
			i++;
		}
		else {
			unsigned int i0, i1, ii;
			sscanf( token, "%u-%u", &i0, &i1 );

			/* make sure the array is big enough to hold the range */
			if ( *array_size < (i + i1 - i0) )  {

				*array_size = arr_resize((void**)array,
						sizeof(int), *array_size,
						i1 - i0 );

				if( *array_size == 0 ) {
					return -1;
				}
			}

			/* add all numbers in the range to the array */
			for ( ii = i0; ii <= i1; ii++ ) {
				(*array)[i++] = ii;
			}
		}

		/* get the next token */
		token = strtok(NULL, ",");
	}
	return i;
}


int get_line_pos( const char *ct, const int field_no, const char *d, int *start, int *end) {
	int i      = 0,
	    len    = strlen(ct),
	    len_d  = strlen(d),
	    field  = 0;

	// Set "not found" values.
	*start = -1;
	*end   = -1;

	// Are we searching for the first field?
	if( field_no == 0 ) {
		*start = 0;
	}

	// Search for the n-th field
	for (i = 0; *start == -1 && i < len; i++ ) {

		// Did we find a delimiter?
		if ( strncmp(ct + i, d, len_d) == 0 ) {
			field += 1;
			if( field == field_no ) {
				*start = i + len_d;
			}
		}
	}

	// Seach in the string
	for( i = *start; *end == -1 && i < len; i++ ) {
	
		// Did we hit the last character of the line?
		if( i == len - 1 ) {
			*end = i;
		}	

		// Did we find a delimiter or linebreak?
		if( strncmp(ct + i, d, len_d) == 0 ||
		    *(ct + i) == '\n' || *(ct + i) == '\r'
		  ) {
			*end = i - 1;
		}
	}

	return *start != -1 && *end != -1 ? 1 : 0;
}


char * cut_field( char *ct, const int i, const char *d) {

	int start = 0, end = 0;
	char *result = NULL;

	if ( get_line_pos(ct, i, d, &start, &end) ) {

		// Is this the first field?
		if(start > 0) {
			
			// No => Adjust the start position to include the last delimiter
			start -= strlen(d);
		}
		// Is this the last field?
		else if ( end < strlen(ct) - 1 ) {

			// No => Adjust the end position to include the next delimiter
			end += strlen(d);
	
		}

	     	// Copy the old to the new string
		result = (char *) malloc(sizeof(char) * (strlen(ct) + 1));
		if ( start > 0 ) {
			strncpy(result, ct, start);
		}
		if ( end - strlen(ct) > 0 ) {
			strncpy(result + start, ct + end + 1, strlen(ct) - end + 1);
		}
	}

	return result;
}


ssize_t field_str( const char *value, const char *line, const char *delim){

	char *curfield;		/* to hold fields from line */
	int   max_field_chars;	/* size of curfield buffer */
	int   curfield_len;	/* return value of get_line_field() */

	int   i;		/* the index of the field being inspected */
	int   found;		/* whether the value was found in line */


	/* no value to look for?  don't waste our time.
	   but looking for an empty string may be valid. */
	if ( value == NULL )
		return -2;

	/* undefined or empty line?  then it can't contain the value. */
	if ( line == NULL || line[0] == '\0' )
		return -1;

	/* no delimiter? then treat the line like a single field. */
	if ( delim == NULL || delim[0] == '\0' ) {
		if ( str_eq(value, line) )
			return 0;
		return -1;
	}

	/* this only needs to be just long enough to see if the
	   field matches value (1 char longer), but making it a little
	   bigger, just for fun.  and allocating max+1 so there's room for
	   the null terminator. */
	max_field_chars = strlen(value) + 3;
	curfield = malloc( max_field_chars + 1 );
	if ( curfield == NULL )
		return -2;

	i = 0;
	curfield_len = 0;
	found = 0;

	while ( (curfield_len = get_line_field(	curfield, line,
						max_field_chars, i, delim)
		) > -1 )
	{
		if ( str_eq(curfield, value) ) {
			found = 1;
			break;
		}

		i++;
	}

	free(curfield);

	if ( found )
		return i;

	return -1;
}

