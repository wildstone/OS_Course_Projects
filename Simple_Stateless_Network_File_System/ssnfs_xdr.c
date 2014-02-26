/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "ssnfs.h"

bool_t
xdr_create_input (XDR *xdrs, create_input *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->user_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->file_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_create_output (XDR *xdrs, create_output *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->out_msg.out_msg_val, (u_int *) &objp->out_msg.out_msg_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_list_input (XDR *xdrs, list_input *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->usrname, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_list_output (XDR *xdrs, list_output *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->out_msg.out_msg_val, (u_int *) &objp->out_msg.out_msg_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_delete_input (XDR *xdrs, delete_input *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->user_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->file_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_delete_output (XDR *xdrs, delete_output *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->out_msg.out_msg_val, (u_int *) &objp->out_msg.out_msg_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_write_input (XDR *xdrs, write_input *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->user_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->file_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->offset))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->numbytes))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->buffer.buffer_val, (u_int *) &objp->buffer.buffer_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_write_output (XDR *xdrs, write_output *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->out_msg.out_msg_val, (u_int *) &objp->out_msg.out_msg_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_read_input (XDR *xdrs, read_input *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->user_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->file_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->offset))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->numbytes))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_read_output (XDR *xdrs, read_output *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->out_msg.out_msg_val, (u_int *) &objp->out_msg.out_msg_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_copy_input (XDR *xdrs, copy_input *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->user_name, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->from_filename, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->to_filename, 10,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_copy_output (XDR *xdrs, copy_output *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->out_msg.out_msg_val, (u_int *) &objp->out_msg.out_msg_len, ~0,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}
