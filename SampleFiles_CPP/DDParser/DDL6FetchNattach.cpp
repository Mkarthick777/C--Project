
#pragma warning (disable : 4786)
#include "pch.h"
#ifndef _WIN32_WCE
#include "std.h"	//removed to stop compiler error win ce PAW 09/04/09
#endif
#include "Flats.h"
#include "DDL6Items.h"
#include "Retn_Code.h"

/*********************************************************************
 *
 *  Name:   attach_chart_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      stevev modified code from David Bradsher
 *
 *********************************************************************/

int
attach_chart_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_CHART     *chart_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	//chart_flat = &(glblFlats.fChart);
	chart_flat = &( ((FLAT_UNION_T*)flats)->fChart );

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the chart.
	 */

	if (!chart_flat->depbin) {

		chart_flat->depbin =
			(CHART_DEPBIN *) new CHART_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) chart_flat->depbin, 0, sizeof(CHART_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{		
	case CHART_LABEL_ID:	{depbin_ptr = &chart_flat->depbin->db_label;  }		break;
	case CHART_HELP_ID:		{depbin_ptr = &chart_flat->depbin->db_help;   }		break;
	case CHART_VALID_ID:	{depbin_ptr = &chart_flat->depbin->db_valid;  }		break;
	case CHART_HEIGHT_ID:	{depbin_ptr = &chart_flat->depbin->db_height; }		break;
	case CHART_WIDTH_ID:	{depbin_ptr = &chart_flat->depbin->db_width;  }		break;
	case CHART_TYPE_ID:		{depbin_ptr = &chart_flat->depbin->db_type;   }		break;
	case CHART_LENGTH_ID:	{depbin_ptr = &chart_flat->depbin->db_length; }		break;
	case CHART_CYCLETIME_ID:{depbin_ptr = &chart_flat->depbin->db_cytime; }		break;
	case CHART_MEMBERS_ID:	{depbin_ptr = &chart_flat->depbin->db_members;}		break;
	case CHART_DEBUG_ID:	{depbin_ptr = &chart_flat->depbin->db_debug_info; }	break;
	default:
		if ( tag >= MAX_CHART_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	chart_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_graph_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      stevev modified code from David Bradsher
 *
 *********************************************************************/

int
attach_graph_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_GRAPH     *graph_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	//chart_flat = &(glblFlats.fChart);
	graph_flat = &( ((FLAT_UNION_T*)flats)->fGraph );

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the chart.
	 */

	if (!graph_flat->depbin) {

		graph_flat->depbin =
			(GRAPH_DEPBIN *) new GRAPH_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) graph_flat->depbin, 0, sizeof(GRAPH_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{		
	case GRAPH_LABEL_ID:	{depbin_ptr = &graph_flat->depbin->db_label;  }		break;
	case GRAPH_HELP_ID:		{depbin_ptr = &graph_flat->depbin->db_help;   }		break;
	case GRAPH_VALID_ID:	{depbin_ptr = &graph_flat->depbin->db_valid;  }		break;
	case GRAPH_HEIGHT_ID:	{depbin_ptr = &graph_flat->depbin->db_height; }		break;
	case GRAPH_WIDTH_ID:	{depbin_ptr = &graph_flat->depbin->db_width;  }		break;
	case GRAPH_XAXIS_ID:	{depbin_ptr = &graph_flat->depbin->db_x_axis; }		break;
	case GRAPH_MEMBERS_ID:	{depbin_ptr = &graph_flat->depbin->db_members;}		break;
	case GRAPH_CYCLETIME_ID:{depbin_ptr = &graph_flat->depbin->db_cytime; }		break;
	case GRAPH_DEBUG_ID:	{depbin_ptr = &graph_flat->depbin->db_debug_info;}	break;
	default:
		if ( tag >= MAX_GRAPH_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	graph_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}

/*Vibhor 260804: Start of Code*/

/*********************************************************************
 *
 *  Name:   attach_axis_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Vibhor modified code from David Bradsher
 *
 *********************************************************************/

int
attach_axis_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_AXIS     *axis_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	axis_flat = &( ((FLAT_UNION_T*)flats)->fAxis );

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the chart.
	 */

	if (!axis_flat->depbin) {

		axis_flat->depbin =
			(AXIS_DEPBIN *) new AXIS_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) axis_flat->depbin, 0, sizeof(AXIS_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{		
	case AXIS_LABEL_ID:		{depbin_ptr = &axis_flat->depbin->db_label;   }		break;
	case AXIS_HELP_ID:		{depbin_ptr = &axis_flat->depbin->db_help;    }		break;
	case AXIS_VALID_ID:		{depbin_ptr = &axis_flat->depbin->db_valid;   }		break;
	case AXIS_MINVAL_ID:	{depbin_ptr = &axis_flat->depbin->db_minval;  }		break;
	case AXIS_MAXVAL_ID:	{depbin_ptr = &axis_flat->depbin->db_maxval;  }		break;
	case AXIS_SCALING_ID:	{depbin_ptr = &axis_flat->depbin->db_scaling; }	    break;
	case AXIS_CONSTUNIT_ID:	{depbin_ptr = &axis_flat->depbin->db_unit;    }		break;
	case AXIS_DEBUG_ID:		{depbin_ptr = &axis_flat->depbin->db_debug_info; }  break;
	default:
		if ( tag >= MAX_AXIS_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	axis_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_file_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Vibhor modified code from David Bradsher
 *
 *********************************************************************/


int
attach_file_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_FILE     *file_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	file_flat = &( ((FLAT_UNION_T*)flats)->fFile );

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the chart.
	 */

	if (!file_flat->depbin) {

		file_flat->depbin =
			(FILE_DEPBIN *) new FILE_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) file_flat->depbin, 0, sizeof(FILE_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{
	case FILE_MEMBERS_ID:	{depbin_ptr = &file_flat->depbin->db_members; }		break;	
	case FILE_LABEL_ID:		{depbin_ptr = &file_flat->depbin->db_label;   }		break;
	case FILE_HELP_ID:		{depbin_ptr = &file_flat->depbin->db_help;    }		break;	
	case FILE_DEBUG_ID:		{depbin_ptr = &file_flat->depbin->db_debug_info;}	break;
	default:
		if ( tag >= MAX_FILE_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	file_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}

/*********************************************************************
 *
 *  Name:   attach_waveform_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Vibhor modified code from David Bradsher
 *
 *********************************************************************/


int
attach_waveform_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_WAVEFORM     *wavfrm_flat;
	DEPBIN			  **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	wavfrm_flat = &( ((FLAT_UNION_T*)flats)->fWaveFrm );

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the chart.
	 */

	if (!wavfrm_flat->depbin) {

		wavfrm_flat->depbin =
			(WAVEFORM_DEPBIN *) new WAVEFORM_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) wavfrm_flat->depbin, 0, sizeof(WAVEFORM_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{
	case WAVEFORM_LABEL_ID:			{depbin_ptr = &wavfrm_flat->depbin->db_label;	  }	break;
	case WAVEFORM_HELP_ID:			{depbin_ptr = &wavfrm_flat->depbin->db_help;	  }	break;
	case WAVEFORM_HANDLING_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_handling;  }	break;
	case WAVEFORM_EMPHASIS_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_emphasis;  }	break;
	case WAVEFORM_LINETYPE_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_linetype;  }	break;
	case WAVEFORM_LINECOLOR_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_linecolor; }	break;
	case WAVEFORM_YAXIS_ID:			{depbin_ptr = &wavfrm_flat->depbin->db_y_axis;	  }	break;
	case WAVEFORM_KEYPTS_X_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_x_keypts;  }	break;
	case WAVEFORM_KEYPTS_Y_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_y_keypts;  }	break;
	case WAVEFORM_TYPE_ID:			{depbin_ptr = &wavfrm_flat->depbin->db_type;	  }	break;
	case WAVEFORM_X_VALUES_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_x_values;  }	break;
	case WAVEFORM_Y_VALUES_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_y_values;  }	break;
	case WAVEFORM_X_INITIAL_ID:		{depbin_ptr = &wavfrm_flat->depbin->db_x_initial; }	break;
	case WAVEFORM_X_INCREMENT_ID:	{depbin_ptr = &wavfrm_flat->depbin->db_x_incr;	  }	break;
	case WAVEFORM_POINT_COUNT_ID:	{depbin_ptr = &wavfrm_flat->depbin->db_pt_count;  }	break;
	case WAVEFORM_INIT_ACTIONS_ID:	{depbin_ptr = &wavfrm_flat->depbin->db_init_acts; }	break;
	case WAVEFORM_RFRSH_ACTIONS_ID:	{depbin_ptr = &wavfrm_flat->depbin->db_rfrsh_acts;}	break;
	case WAVEFORM_EXIT_ACTIONS_ID:	{depbin_ptr = &wavfrm_flat->depbin->db_exit_acts; }	break;
	case WAVEFORM_DEBUG_ID:			{depbin_ptr = &wavfrm_flat->depbin->db_debug_info;}	break;
	case WAVEFORM_VALID_ID:			{depbin_ptr = &wavfrm_flat->depbin->db_valid;	  }	break;
	default:
		if ( tag >= MAX_WAVEFORM_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	wavfrm_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}

/*Vibhor 260804: End of Code*/

/*Vibhor 290904: Start of Code*/
/*********************************************************************
 *
 *  Name:   attach_list_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Vibhor modified code from David Bradsher
 *
 *********************************************************************/


int
attach_list_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_LIST     *list_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	list_flat = &( ((FLAT_UNION_T*)flats)->fList);

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the chart.
	 */

	if (!list_flat->depbin) {

		list_flat->depbin =
			(LIST_DEPBIN *) new LIST_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) list_flat->depbin, 0, sizeof(LIST_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{
	case LIST_LABEL_ID:		{depbin_ptr = &list_flat->depbin->db_label;   }		break;
	case LIST_HELP_ID:		{depbin_ptr = &list_flat->depbin->db_help;    }		break;
	case LIST_VALID_ID:		{depbin_ptr = &list_flat->depbin->db_valid;   }		break;
	case LIST_TYPE_ID:		{depbin_ptr = &list_flat->depbin->db_type;    }		break;
	case LIST_COUNT_ID:		{depbin_ptr = &list_flat->depbin->db_count;   }		break;
	case LIST_CAPACITY_ID:	{depbin_ptr = &list_flat->depbin->db_capacity;}		break;
	case LIST_DEBUG_ID:		{depbin_ptr = &list_flat->depbin->db_debug_info; }	break;
	default:
		if ( tag >= MAX_LIST_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	list_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_source_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Vibhor modified code from David Bradsher
 *
 *********************************************************************/


int
attach_source_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_SOURCE     *source_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	source_flat = &( ((FLAT_UNION_T*)flats)->fSource);

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the chart.
	 */

	if (!source_flat->depbin) {

		source_flat->depbin =
			(SOURCE_DEPBIN *) new SOURCE_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) source_flat->depbin, 0, sizeof(SOURCE_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{
	case SOURCE_LABEL_ID:		 {depbin_ptr = &source_flat->depbin->db_label;     }	break;
	case SOURCE_HELP_ID:		 {depbin_ptr = &source_flat->depbin->db_help;      }	break;
	case SOURCE_VALID_ID:		 {depbin_ptr = &source_flat->depbin->db_valid;     }	break;
	case SOURCE_EMPHASIS_ID:     {depbin_ptr = &source_flat->depbin->db_emphasis;  }	break;
	case SOURCE_LINETYPE_ID:	 {depbin_ptr = &source_flat->depbin->db_linetype;  }	break;
	case SOURCE_LINECOLOR_ID:	 {depbin_ptr = &source_flat->depbin->db_linecolor; }	break;
	case SOURCE_YAXIS_ID:		 {depbin_ptr = &source_flat->depbin->db_y_axis;    }	break;
	case SOURCE_INIT_ACTIONS_ID: {depbin_ptr = &source_flat->depbin->db_init_acts; }	break;
	case SOURCE_RFRSH_ACTIONS_ID:{depbin_ptr = &source_flat->depbin->db_rfrsh_acts;}	break;
	case SOURCE_EXIT_ACTIONS_ID: {depbin_ptr = &source_flat->depbin->db_exit_acts; }	break;
	case SOURCE_MEMBERS_ID:		 {depbin_ptr = &source_flat->depbin->db_members;   }	break;
	case SOURCE_DEBUG_ID:		 {depbin_ptr = &source_flat->depbin->db_debug_info;}	break;	
	default:
		if ( tag >= MAX_SOURCE_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	source_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}



/*Vibhor 290904: End of Code*/


/* stevev 22mar05 - start of grid-image item */

/*********************************************************************
 *
 *  Name:   attach_image_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Stevev from Vibhor modified code from David Bradsher
 *
 *********************************************************************/


int
attach_image_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_IMAGE     *image_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	image_flat = &( ((FLAT_UNION_T*)flats)->fImage);

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the image.
	 */

	if (!image_flat->depbin) {

		image_flat->depbin =
			(IMAGE_DEPBIN *) new IMAGE_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) image_flat->depbin, 0, sizeof(IMAGE_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{
	case IMAGE_LABEL_ID:	{depbin_ptr = &image_flat->depbin->db_label;   }		break;
	case IMAGE_HELP_ID:		{depbin_ptr = &image_flat->depbin->db_help;    }		break;
	case IMAGE_VALID_ID:	{depbin_ptr = &image_flat->depbin->db_valid;   }		break;
	case IMAGE_LINK_ID:		{depbin_ptr = &image_flat->depbin->db_link;    }		break;
	case IMAGE_PATH_ID:		{depbin_ptr = &image_flat->depbin->db_path;    }		break;
	case IMAGE_DEBUG_ID:	{depbin_ptr = &image_flat->depbin->db_debug_info;}		break;
	default:
		if ( tag >= MAX_IMAGE_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	image_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_grid_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Stevev from Vibhor modified code from David Bradsher
 *
 *********************************************************************/


int
attach_grid_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_GRID     *grid_flat;
	DEPBIN        **depbin_ptr;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	grid_flat = &( ((FLAT_UNION_T*)flats)->fGrid);

	/*
	 * Check the flat structure for existence of the DEPBIN pointer chart.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the grid.
	 */

	if (!grid_flat->depbin) {

		grid_flat->depbin =
			(GRID_DEPBIN *) new GRID_DEPBIN;
		/*
		 * Force the DEPBIN pointer chart to all 0's
		 */

		(void)memset((char *) grid_flat->depbin, 0, sizeof(GRID_DEPBIN));//ok
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN chart.
	 */

	switch (tag) 
	{
	case GRID_LABEL_ID:		{depbin_ptr = &grid_flat->depbin->db_label;   }		break;
	case GRID_HELP_ID:		{depbin_ptr = &grid_flat->depbin->db_help;    }		break;
	case GRID_VALID_ID:		{depbin_ptr = &grid_flat->depbin->db_valid;   }		break;
		
	case GRID_HEIGHT_ID:	{depbin_ptr = &grid_flat->depbin->db_height;  }		break;
	case GRID_WIDTH_ID:		{depbin_ptr = &grid_flat->depbin->db_width;   }		break;
	case GRID_ORIENT_ID:	{depbin_ptr = &grid_flat->depbin->db_orient;  }		break;
	case GRID_HANDLING_ID:	{depbin_ptr = &grid_flat->depbin->db_handling;}		break;
	case GRID_MEMBERS_ID:	{depbin_ptr = &grid_flat->depbin->db_members; }		break;
	case GRID_DEBUG_ID:		{depbin_ptr = &grid_flat->depbin->db_debug_info;}	break;


	default:
		if ( tag >= MAX_GRID_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) 
	{
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));//ok
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size  = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	grid_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/* stevev 22mar05 - end of grid-image code */
