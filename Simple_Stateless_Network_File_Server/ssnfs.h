/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _SSNFS_H_RPCGEN
#define _SSNFS_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct create_input {
	char user_name[10];
	char file_name[10];
};
typedef struct create_input create_input;

struct create_output {
	struct {
		u_int out_msg_len;
		char *out_msg_val;
	} out_msg;
};
typedef struct create_output create_output;

struct list_input {
	char usrname[10];
};
typedef struct list_input list_input;

struct list_output {
	struct {
		u_int out_msg_len;
		char *out_msg_val;
	} out_msg;
};
typedef struct list_output list_output;

struct delete_input {
	char user_name[10];
	char file_name[10];
};
typedef struct delete_input delete_input;

struct delete_output {
	struct {
		u_int out_msg_len;
		char *out_msg_val;
	} out_msg;
};
typedef struct delete_output delete_output;

struct write_input {
	char user_name[10];
	char file_name[10];
	int offset;
	int numbytes;
	struct {
		u_int buffer_len;
		char *buffer_val;
	} buffer;
};
typedef struct write_input write_input;

struct write_output {
	struct {
		u_int out_msg_len;
		char *out_msg_val;
	} out_msg;
};
typedef struct write_output write_output;

struct read_input {
	char user_name[10];
	char file_name[10];
	int offset;
	int numbytes;
};
typedef struct read_input read_input;

struct read_output {
	struct {
		u_int out_msg_len;
		char *out_msg_val;
	} out_msg;
};
typedef struct read_output read_output;

struct copy_input {
	char user_name[10];
	char from_filename[10];
	char to_filename[10];
};
typedef struct copy_input copy_input;

struct copy_output {
	struct {
		u_int out_msg_len;
		char *out_msg_val;
	} out_msg;
};
typedef struct copy_output copy_output;

#define SSNFSPROG 0x31110023
#define SSNFSVER 1

#if defined(__STDC__) || defined(__cplusplus)
#define create_file 1
extern  create_output * create_file_1(create_input *, CLIENT *);
extern  create_output * create_file_1_svc(create_input *, struct svc_req *);
#define list_files 2
extern  list_output * list_files_1(list_input *, CLIENT *);
extern  list_output * list_files_1_svc(list_input *, struct svc_req *);
#define delete_file 3
extern  delete_output * delete_file_1(delete_input *, CLIENT *);
extern  delete_output * delete_file_1_svc(delete_input *, struct svc_req *);
#define write_file 4
extern  write_output * write_file_1(write_input *, CLIENT *);
extern  write_output * write_file_1_svc(write_input *, struct svc_req *);
#define read_file 5
extern  read_output * read_file_1(read_input *, CLIENT *);
extern  read_output * read_file_1_svc(read_input *, struct svc_req *);
#define copy_file 6
extern  copy_output * copy_file_1(copy_input *, CLIENT *);
extern  copy_output * copy_file_1_svc(copy_input *, struct svc_req *);
extern int ssnfsprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define create_file 1
extern  create_output * create_file_1();
extern  create_output * create_file_1_svc();
#define list_files 2
extern  list_output * list_files_1();
extern  list_output * list_files_1_svc();
#define delete_file 3
extern  delete_output * delete_file_1();
extern  delete_output * delete_file_1_svc();
#define write_file 4
extern  write_output * write_file_1();
extern  write_output * write_file_1_svc();
#define read_file 5
extern  read_output * read_file_1();
extern  read_output * read_file_1_svc();
#define copy_file 6
extern  copy_output * copy_file_1();
extern  copy_output * copy_file_1_svc();
extern int ssnfsprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_create_input (XDR *, create_input*);
extern  bool_t xdr_create_output (XDR *, create_output*);
extern  bool_t xdr_list_input (XDR *, list_input*);
extern  bool_t xdr_list_output (XDR *, list_output*);
extern  bool_t xdr_delete_input (XDR *, delete_input*);
extern  bool_t xdr_delete_output (XDR *, delete_output*);
extern  bool_t xdr_write_input (XDR *, write_input*);
extern  bool_t xdr_write_output (XDR *, write_output*);
extern  bool_t xdr_read_input (XDR *, read_input*);
extern  bool_t xdr_read_output (XDR *, read_output*);
extern  bool_t xdr_copy_input (XDR *, copy_input*);
extern  bool_t xdr_copy_output (XDR *, copy_output*);

#else /* K&R C */
extern bool_t xdr_create_input ();
extern bool_t xdr_create_output ();
extern bool_t xdr_list_input ();
extern bool_t xdr_list_output ();
extern bool_t xdr_delete_input ();
extern bool_t xdr_delete_output ();
extern bool_t xdr_write_input ();
extern bool_t xdr_write_output ();
extern bool_t xdr_read_input ();
extern bool_t xdr_read_output ();
extern bool_t xdr_copy_input ();
extern bool_t xdr_copy_output ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_SSNFS_H_RPCGEN */