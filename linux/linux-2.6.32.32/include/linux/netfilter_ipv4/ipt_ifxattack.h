/* ============================================================================
 * Copyright (C) 2003[- 2004] ? Infineon Technologies AG.
 *
 * All rights reserved.
 * ============================================================================
 *
 * ============================================================================
 *
 * This document contains proprietary information belonging to Infineon 
 * Technologies AG. Passing on and copying of this document, and communication
 * of its contents is not permitted without prior written authorisation.
 * 
 * ============================================================================
 */


/* ===========================================================================
 *
 * File Name: ipt_ifxattack.h
 * Author : Nirav Salot
 * Date: 
 *
 * ===========================================================================
 *
 * Project: <project/component name>
 * Block: <block/module name>
 *
 * ===========================================================================
 * Contents: This file contains the common definitions required by the
 * 	     ifxattack module.
 * 
 * ===========================================================================
 * References: <List of design documents covering this file.>
 */
#ifndef _IPT_IFXATTACK_MATCH_H
#define _IPT_IFXATTACK_MATCH_H

/*
#define	CONFIG_CODE_REDI
#define CONFIG_CODE_REDII
#define CONFIG_LAND_ATTACK
#define CONFIG_ICQ_DOS
#define CONFIG_MALFORMED_MIME
//#define CONFIG_IIS_FRONTPAGE
#define CONFIG_UDP_BOMB
#define CONFIG_FTP_PORT_REST
*/

enum ifxattack_name { 
	CODE_REDI = 111 ,
	CODE_REDII = 222,
	LAND_ATTACK = 333 ,
	ICQ_DOS = 444 ,
	MALFORMED_MIME = 555,
	IIS_FRONTPAGE = 666 ,
	UDP_BOMB = 777,
	FTP_PORT_REST = 888
};

struct ipt_ifxattack_match_info {
	enum ifxattack_name attack_name;
};

#endif /*_IPT_IFXATTACK_MATCH_H */

