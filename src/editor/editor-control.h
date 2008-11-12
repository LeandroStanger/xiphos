/*
 * GnomeSword Bible Study Tool
 * editor-control.c - the html editor
 *
 * Copyright (C) 2004, 2005 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _EDITOR_CONTROL_H
#define _EDITOR_CONTROL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <config.h>	
#ifndef USE_GTKHTML38
	
#include "editor/editor.h"	
enum {
	STUDYPAD,
	PERCOM
};

GSHTMLEditorControlData *editor_new(GtkWidget * container, int type, char * filename);

#endif /* !USE_GTKHTML38 */

#ifdef __cplusplus
}
#endif

#endif /* _EDITOR_CONTROL_H */