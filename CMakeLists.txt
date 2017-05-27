cmake_minimum_required( VERSION 3.0 )

project( TiMidity )

include( CheckFunctionExists )

# TODO: detect values
set( DEFAULT_PATH "\"${CMAKE_INSTALL_PREFIX}/share/timidity\"" )
set( PACKAGE_VERSION "\"2.14.0\"" )
set( RETSIGTYPE "void" )
set( TIMIDITY_OUTPUT_ID "\"d\"" )

CHECK_FUNCTION_EXISTS( alarm HAVE_ALARM )
CHECK_FUNCTION_EXISTS( dlopen HAVE_DLOPEN )
CHECK_FUNCTION_EXISTS( dup2 HAVE_DUP2 )
CHECK_FUNCTION_EXISTS( floor HAVE_FLOOR )
CHECK_FUNCTION_EXISTS( fork HAVE_FORK )
CHECK_FUNCTION_EXISTS( getcwd HAVE_GETCWD )
CHECK_FUNCTION_EXISTS( gethostbyname HAVE_GETHOSTBYNAME )
CHECK_FUNCTION_EXISTS( getopt HAVE_GETOPT )
CHECK_FUNCTION_EXISTS( getopt_long HAVE_GETOPT_LONG )
CHECK_FUNCTION_EXISTS( getpagesize HAVE_GETPAGESIZE )
CHECK_FUNCTION_EXISTS( gettimeofday HAVE_GETTIMEOFDAY )
CHECK_FUNCTION_EXISTS( getwd HAVE_GETWD )
CHECK_FUNCTION_EXISTS( isatty HAVE_ISATTY )
CHECK_FUNCTION_EXISTS( memchr HAVE_MEMCHR )
CHECK_FUNCTION_EXISTS( memmove HAVE_MEMMOVE )
CHECK_FUNCTION_EXISTS( memset HAVE_MEMSET )
CHECK_FUNCTION_EXISTS( mkstemp HAVE_MKSTEMP )
CHECK_FUNCTION_EXISTS( mmap HAVE_MMAP )
CHECK_FUNCTION_EXISTS( modf HAVE_MODF )
CHECK_FUNCTION_EXISTS( munmap HAVE_MUNMAP )
CHECK_FUNCTION_EXISTS( open_memstream HAVE_OPEN_MEMSTREAM )
CHECK_FUNCTION_EXISTS( popen HAVE_POPEN )
CHECK_FUNCTION_EXISTS( pow HAVE_POW )
CHECK_FUNCTION_EXISTS( select HAVE_SELECT )
CHECK_FUNCTION_EXISTS( signal HAVE_SIGNAL )
CHECK_FUNCTION_EXISTS( sleep HAVE_SLEEP )
CHECK_FUNCTION_EXISTS( snd_seq_port_info_set_timestamping HAVE_SND_SEQ_PORT_INFO_SET_TIMESTAMPING )
CHECK_FUNCTION_EXISTS( snprintf HAVE_SNPRINTF )
CHECK_FUNCTION_EXISTS( socket HAVE_SOCKET )
CHECK_FUNCTION_EXISTS( sqrt HAVE_SQRT )
CHECK_FUNCTION_EXISTS( strchr HAVE_STRCHR )
CHECK_FUNCTION_EXISTS( strdup HAVE_STRDUP )
CHECK_FUNCTION_EXISTS( strerror HAVE_STRERROR )
CHECK_FUNCTION_EXISTS( strlcat HAVE_STRLCAT )
CHECK_FUNCTION_EXISTS( strlcpy HAVE_STRLCPY )
CHECK_FUNCTION_EXISTS( strncasecmp HAVE_STRNCASECMP )
CHECK_FUNCTION_EXISTS( strncasecmp HAVE_STRNCASECMP )
CHECK_FUNCTION_EXISTS( strstr HAVE_STRSTR )
CHECK_FUNCTION_EXISTS( strtol HAVE_STRTOL )
CHECK_FUNCTION_EXISTS( usleep HAVE_USLEEP )
CHECK_FUNCTION_EXISTS( vfork HAVE_VFORK )
CHECK_FUNCTION_EXISTS( vprintf HAVE_VPRINTF )
CHECK_FUNCTION_EXISTS( vsnprintf HAVE_VSNPRINTF )
CHECK_FUNCTION_EXISTS( vwprintw HAVE_VWPRINTW )
CHECK_FUNCTION_EXISTS( XmuRegisterExternalAgent HAVE_XMUREGISTEREXTERNALAGENT )
CHECK_FUNCTION_EXISTS( XShmCreatePixmap HAVE_XSHMCREATEPIXMAP )

include( CheckIncludeFile )

CHECK_INCLUDE_FILE( arpa/inet.h HAVE_ARPA_INET_H )
CHECK_INCLUDE_FILE( curses.h HAVE_CURSES_H )
CHECK_INCLUDE_FILE( dirent.h HAVE_DIRENT_H )
CHECK_INCLUDE_FILE( dlfcn.h HAVE_DLFCN_H )
CHECK_INCLUDE_FILE( errno.h HAVE_ERRNO_H )
CHECK_INCLUDE_FILE( fcntl.h HAVE_FCNTL_H )
CHECK_INCLUDE_FILE( getopt.h HAVE_GETOPT_H )
CHECK_INCLUDE_FILE( glob.h HAVE_GLOB_H )
CHECK_INCLUDE_FILE( inttypes.h HAVE_INTTYPES_H )
CHECK_INCLUDE_FILE( limits.h HAVE_LIMITS_H )
CHECK_INCLUDE_FILE( machine/endian.h HAVE_MACHINE_ENDIAN_H )
CHECK_INCLUDE_FILE( malloc.h HAVE_MALLOC_H )
CHECK_INCLUDE_FILE( memory.h HAVE_MEMORY_H )
CHECK_INCLUDE_FILE( mmsystem.h HAVE_NEW_MMSYSTEM )
CHECK_INCLUDE_FILE( ncurses.h HAVE_NCURSES_H )
CHECK_INCLUDE_FILE( ncurses/curses.h HAVE_NCURSES_CURSES_H )
CHECK_INCLUDE_FILE( ndir.h HAVE_NDIR_H )
CHECK_INCLUDE_FILE( netdb.h HAVE_NETDB_H )
CHECK_INCLUDE_FILE( netinet/in.h HAVE_NETINET_IN_H )
CHECK_INCLUDE_FILE( nlist.h HAVE_NLIST_H )
CHECK_INCLUDE_FILE( slang.h HAVE_SLANG_H )
CHECK_INCLUDE_FILE( slang/slang.h HAVE_SLANG_SLANG_H )
CHECK_INCLUDE_FILE( soundcard.h HAVE_SOUNDCARD_H )
CHECK_INCLUDE_FILE( stddef.h HAVE_STDDEF_H )
CHECK_INCLUDE_FILE( stdint.h HAVE_STDINT_H )
CHECK_INCLUDE_FILE( stdlib.h HAVE_STDLIB_H )
CHECK_INCLUDE_FILE( string.h HAVE_STRING_H )
CHECK_INCLUDE_FILE( strings.h HAVE_STRINGS_H )
CHECK_INCLUDE_FILE( stropts.h HAVE_STROPTS_H )
CHECK_INCLUDE_FILE( sys/asoundlib.h HAVE_SYS_ASOUNDLIB_H )
CHECK_INCLUDE_FILE( sys/audioio.h HAVE_SYS_AUDIOIO_H )
CHECK_INCLUDE_FILE( sys/awe_voice.h HAVE_SYS_AWE_VOICE_H )
CHECK_INCLUDE_FILE( sys/dir.h HAVE_SYS_DIR_H )
CHECK_INCLUDE_FILE( sys/ioctl.h HAVE_SYS_IOCTL_H )
CHECK_INCLUDE_FILE( sys/ipc.h HAVE_SYS_IPC_H )
CHECK_INCLUDE_FILE( sys/ndir.h HAVE_SYS_NDIR_H )
CHECK_INCLUDE_FILE( sys/param.h HAVE_SYS_PARAM_H )
CHECK_INCLUDE_FILE( sys/shm.h HAVE_SYS_SHM_H )
CHECK_INCLUDE_FILE( sys/socket.h HAVE_SYS_SOCKET_H )
CHECK_INCLUDE_FILE( sys/soundcard.h HAVE_SYS_SOUNDCARD_H )
CHECK_INCLUDE_FILE( sys/stat.h HAVE_SYS_STAT_H )
CHECK_INCLUDE_FILE( sys/sysctl.h HAVE_SYS_SYSCTL_H )
CHECK_INCLUDE_FILE( sys/time.h HAVE_SYS_TIME_H )
CHECK_INCLUDE_FILE( sys/types.h HAVE_SYS_TYPES_H )
CHECK_INCLUDE_FILE( sys/wait.h HAVE_SYS_WAIT_H )
CHECK_INCLUDE_FILE( termios.h HAVE_TERMIOS_H )
CHECK_INCLUDE_FILE( unistd.h HAVE_UNISTD_H )
CHECK_INCLUDE_FILE( vfork.h HAVE_VFORK_H )
CHECK_INCLUDE_FILE( X11/extensions/XShm.h HAVE_X11_EXTENSIONS_XSHM_H )
CHECK_INCLUDE_FILE( X11/Xaw3d/Tip.h HAVE_XAW3D_TIP )
CHECK_INCLUDE_FILE( X11/Xlib.h HAVE_X11_XLIB_H )
CHECK_INCLUDE_FILE( X11/Xmu/ExtAgent.h HAVE_X11_XMU_EXTAGENT_H )

# TODO: 
# - HAVE_GTK_2
# - HAVE_DOPRNT
# - HAVE_LIB...
# - HAVE_SSIZE_T
# - HAVE_STRINGIZE
# - HAVE_UNION_SEMUN
# - HAVE_WORKING_FORK
# - HAVE_WORKING_VFORK

configure_file( config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" )

# TODO: generate interface.h
file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/interface.h" "" )

add_definitions( -DHAVE_CONFIG_H -DNDEBUG )

if( APPLE )
	add_definitions( -DAU_DARWIN )
else()
	# TODO: other platforms
endif()

include_directories( ${CMAKE_CURRENT_BINARY_DIR} interface libarc libunimod timidity utils )

file( GLOB LIBINTERFACE_HEADERS interface/*.h )
set( LIBINTERFACE_SOURCES
	interface/dumb_c.c
	interface/wrdt_dumb.c
	interface/wrdt_tty.c
)
add_library( interface 
	${LIBINTERFACE_SOURCES} 
	${LIBINTERFACE_HEADERS}
)

file( GLOB LIBARC_HEADERS libarc/*.h )
set( LIBARC_SOURCES
	libarc/arc.c
	libarc/arc_lzh.c
	libarc/arc_mime.c
	libarc/arc_tar.c
	libarc/arc_zip.c
	libarc/deflate.c
	libarc/explode.c
	libarc/inflate.c
	libarc/unlzh.c
	libarc/url.c
	libarc/url_b64decode.c
	libarc/url_buff.c
	libarc/url_cache.c
	libarc/url_dir.c
	libarc/url_file.c
	libarc/url_hqxdecode.c
	libarc/url_inflate.c
	libarc/url_mem.c
	libarc/url_pipe.c
	libarc/url_qsdecode.c
	libarc/url_uudecode.c
	# TODO: check other files
)
add_library( arc
	${LIBARC_SOURCES}
	${LIBARC_HEADERS}
)

file( GLOB LIBUNIMOD_HEADERS libunimod/*.h )
set( LIBUNIMOD_SOURCES
	libunimod/load_669.c
	libunimod/load_amf.c
	libunimod/load_dsm.c
	libunimod/load_far.c
	libunimod/load_gdm.c
	libunimod/load_imf.c
	libunimod/load_it.c
	libunimod/load_m15.c
	libunimod/load_med.c
	libunimod/load_mod.c
	libunimod/load_mtm.c
	libunimod/load_okt.c
	libunimod/load_s3m.c
	libunimod/load_stm.c
	libunimod/load_stx.c
	libunimod/load_ult.c
	libunimod/load_uni.c
	libunimod/load_xm.c
	libunimod/mloader.c
	libunimod/mlutil.c
	libunimod/mmsup.c
	libunimod/munitrk.c
	# TODO: check other files
)
add_library( unimod
	${LIBUNIMOD_SOURCES}
	${LIBUNIMOD_HEADERS}
)

file( GLOB LIBUTILS_HEADERS utils/*.h )
set( LIBUTILS_SOURCES
	utils/bitset.c
	utils/fft4g.c
	utils/mblock.c
	utils/memb.c
	utils/nkflib.c
	utils/strtab.c
	utils/support.c
	utils/timer.c
	# TODO: check other files
)
add_library( utils
	${LIBUTILS_SOURCES}
	${LIBUTILS_HEADERS}
)

add_executable( calcnewt
	timidity/calcnewt.c
)

add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/newton_table.c"
	COMMAND calcnewt > "${CMAKE_CURRENT_BINARY_DIR}/newton_table.c"
	DEPENDS calcnewt )

if ( APPLE )
	set( PLATFORM_SOURCES timidity/darwin_a.c )
else()
	# TODO: other platforms
endif()

file( GLOB TIMIDITY_HEADERS timidity/*.h )
set( TIMIDITY_SOURCES
	timidity/aiff_a.c
	timidity/aq.c
	timidity/au_a.c
	timidity/audio_cnv.c
	timidity/common.c
	timidity/controls.c
	timidity/effect.c
	timidity/filter.c
	timidity/freq.c
	timidity/instrum.c
	timidity/list_a.c
	timidity/loadtab.c
	timidity/m2m.c
	timidity/mfi.c
	timidity/midi_a.c
	timidity/miditrace.c
	timidity/mix.c
	timidity/mod.c
	timidity/mod2midi.c
	timidity/modmid_a.c
	timidity/mt19937ar.c
	timidity/optcode.c
	timidity/output.c
	timidity/playmidi.c
	timidity/quantity.c
	timidity/raw_a.c
	timidity/rcp.c
	timidity/readmidi.c
	timidity/recache.c
	timidity/resample.c
	timidity/reverb.c
	timidity/sbkconv.c
	timidity/sffile.c
	timidity/sfitem.c
	timidity/smfconv.c
	timidity/smplfile.c
	timidity/sndfont.c
	timidity/tables.c
	timidity/timidity.c
	timidity/version.c
	timidity/wave_a.c
	timidity/wrd_read.c
	timidity/wrdt.c
	# TODO: check other files
	${PLATFORM_SOURCES}
)
add_executable( timidity
	${TIMIDITY_SOURCES}
	${TIMIDITY_HEADERS}
)

set_source_files_properties( timidity/resample.c PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/newton_table.c" )

target_link_libraries( timidity interface arc unimod utils )

if( APPLE )
	set_target_properties( timidity PROPERTIES LINK_FLAGS "-framework CoreAudio" )
else()
	# TODO: other platforms
endif()
