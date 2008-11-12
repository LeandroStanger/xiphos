/*
 * GnomeSword Bible Study Tool
 * editor_menu.c - popup menu for the html editor
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef USE_GTKHTML38
#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/gtkhtmlfontstyle.h>
#include <gtkhtml/htmlform.h>
#include <gtkhtml/htmlenums.h>
#include <gtkhtml/htmlsettings.h>
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#ifdef USE_GTKHTML30
#include <gtkhtml/htmllinktext.h>
#endif
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
/*
#ifdef USE_GTKHTML30
#include <gal/widgets/e-unicode.h>
#endif
*/
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "editor/editor.h"
#include "editor/toolbar_style.h"
#include "editor/editor_menu.h"
//#include "editor/editor_replace.h"
//#include "editor/editor_spell.h"
#include "gui/fileselection.h"
#include "gui/studypad.h"
#include "gui/html.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/dialog.h"
#include "gui/find_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/xml.h"
#include "main/module_dialogs.h"


/******************************************************************************
 * Name
 *  gui_new_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void gui_new_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    ask to save old file if needed then create new
 *
 * Return value
 *   void
 */

void gui_new_activate(GtkMenuItem * menuitem,
		      GSHTMLEditorControlData * ecd)
{
	gchar *filename = NULL;
	gchar *buf = NULL;
	gint test;
	GS_DIALOG *info;
	GString *str;
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			buf,
			"has been modified. Do you wish to save it?");
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			if (settings.studypadfilename) {
				filename = g_strdup(settings.studypadfilename);
				save_file(filename, ecd);
			} else {
				gui_fileselection_save(ecd,TRUE);
			}
		}
		settings.modifiedSP = FALSE;
		g_free(info);
		g_string_free(str,TRUE);
	}
	
	
	settings.studypadfilename = NULL;
	xml_set_value("GnomeSword", "studypad", "lastfile", 
							NULL);
	
	sprintf(ecd->filename, "%s", "");
	ecd->filename[0] = '\0';
	gtk_html_select_all(ecd->html);
	gtk_html_cut(ecd->html);
	gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), 1,
			   _("-untitled-"));
	ecd->changed = FALSE;
}

/******************************************************************************
 * Name
 *  on_open_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_open_activate(GtkMenuItem * menuitem,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open a file into notepad
 *
 * Return value
 *   void
 */

static void on_open_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	GtkWidget *openFile;
	gchar buf[255];
	gchar *tmp_buf = NULL;
	GString *str;
	gchar *filename = NULL;
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		GS_DIALOG *info;
		gint test;

		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			tmp_buf = settings.studypadfilename;
		else
			tmp_buf = N_("File");
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			tmp_buf,
			"has been modified. Do you wish to save it?");
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_gs_dialog(info);
		if (test == GS_YES) {
			if (settings.studypadfilename) {
				filename = g_strdup(settings.studypadfilename);
				save_file(filename, ecd);
			} else {
				gui_fileselection_save(ecd,TRUE);
			}
		}
		g_free(info);
		g_string_free(str,TRUE);
	}
	sprintf(buf, "%s/*.pad", settings.studypaddir);
	openFile = gui_fileselection_open(ecd);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile),
					buf);
	gtk_widget_show(openFile);
}


/******************************************************************************
 * Name
 *  on_save_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_save_activate(GtkMenuItem * menuitem,
				      GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    if filename call save_file() else open save file dialog
 *
 * Return value
 *   void
 */

void on_save_activate(GtkMenuItem * menuitem,
		      GSHTMLEditorControlData * ecd)
{
	if (ecd->filename) {
		save_file(ecd->filename, ecd);
		return;
	} else {
		gui_fileselection_save(ecd,TRUE);
	}
}


/******************************************************************************
 * Name
 *  on_export_plain_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_export_plain_activate(GtkMenuItem * menuitem,
				      GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    if filename call save_file_plain_text() else open save file dialog
 *
 * Return value
 *   void
 */

void on_export_plain_activate(GtkMenuItem * menuitem,
		      GSHTMLEditorControlData * ecd)
{
	gui_fileselection_save(ecd,FALSE);
}


/******************************************************************************
 * Name
 *  on_save_as_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_save_as_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    call create_fileselectionSave() to save file as
 *
 * Return value
 *   void
 */

static void on_save_as_activate(GtkMenuItem * menuitem,
				GSHTMLEditorControlData * ecd)
{
	gui_fileselection_save(ecd,TRUE);
}

/******************************************************************************
 * Name
 *  on_print_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_print_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    print content of editor to printer
 *
 * Return value
 *   void
 */

static void on_print_activate(GtkMenuItem * menuitem,
			      GSHTMLEditorControlData * ecd)
{
	gui_html_print(ecd->htmlwidget, FALSE);	/* gs_html.c */
}

/******************************************************************************
 * Name
 *  on_cut_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_cut_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    cut selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_cut_activate(GtkMenuItem * menuitem,
			    GSHTMLEditorControlData * ecd)
{
	gtk_html_cut(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gtk_html_copy(ecd->html);
}
 
/******************************************************************************
 * Name
 *  on_paste_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_paste_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    paste contents of clipboard into editor
 *
 * Return value
 *   void
 */

static void on_paste_activate(GtkMenuItem * menuitem,
			      GSHTMLEditorControlData * ecd)
{
	gtk_html_paste(ecd->html,FALSE);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_undo_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_undo_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    undo changes to editor
 *
 * Return value
 *   void
 */

static void on_undo_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gtk_html_undo(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_find_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open find dialog
 *
 * Return value
 *   void
 */

static void on_find_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gui_find_dlg(ecd->htmlwidget, ecd->filename,
		  FALSE, NULL);
	//search(ecd, FALSE, NULL);
}


/******************************************************************************
 * Name
 *  on_replace_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_replace_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    opens find and replace dialog
 *
 * Return value
 *   void
 */

static void on_replace_activate(GtkMenuItem * menuitem,
				GSHTMLEditorControlData * ecd)
{
	replace(ecd);
}

/******************************************************************************
 * Name
 *  set_link_to_module
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void set_link_to_module(gchar * linkref, gchar * linkmod,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set link to module and key
 *
 * Return value
 *   void
 */

static void set_link_to_module(gchar * linktext, gchar * linkref, 
			       gchar * linkmod, GSHTMLEditorControlData * ecd)
{
	const gchar *url;
	const gchar *text;
	gchar *target;
	gchar *url_copy;
	gchar buf[256];
	//HTMLEngine *e;
	gchar *new_link = NULL;

	//e = ecd->html->engine;
	
	if (linkmod && strlen(linkmod))
		sprintf(buf, "sword://%s/%s", linkmod, linkref);
	else
		sprintf(buf, "<a href=\"sword:///%s\">", linkref);

	url = buf;
	text = linkref;
	if (url && text && *url && *text) {
		target = strchr(url, '#');
		url_copy =
		    target ? g_strndup(url,
				       target - url) : g_strdup(url);
		new_link = g_strdup_printf("<a href=\"%s\">%s</a>",url_copy,linktext);
		gtk_html_insert_html(ecd->html,new_link);
		g_free(url_copy);
		g_free(new_link);
	}
}

/******************************************************************************
 * Name
 *  on_link_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_link_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    opens link dialog
 *
 * Return value
 *   void
 */

static void on_link_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gint test;
	GS_DIALOG *info;
	GString *str;
	
	str = g_string_new(NULL);
	g_string_printf(str,"<span weight=\"bold\">%s</span>",_("Add reference Link"));

	info = gui_new_dialog();
	//info->stock_icon = GTK_STOCK_ADD;
	info->label_top = str->str;
	info->text1 = g_strdup("");
	info->label1 = N_("Link text: ");
	info->text2 = g_strdup("");
	info->label2 = N_("Reference: ");
	info->text3 = g_strdup("");
	info->label3 = N_("Module: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	
	 // * get selected text *
	
	if (html_engine_is_selection_active(ecd->html->engine)) {
		gchar *buf;
		buf =
		    html_engine_get_selection_string(ecd->html->engine);
		info->text1 = g_strdup(buf);
		info->text2 = g_strdup(buf);
	}
	info->text3 = g_strdup(xml_get_value("modules", "bible"));//settings.MainWindowModule);
	// *** open dialog to get name for list ***
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		if (strlen(info->text1) > 0) {
			set_link_to_module(info->text1, info->text2, info->text3,
					   ecd);
			ecd->changed = TRUE;
			gui_update_statusbar(ecd);
		}
	}
	g_free(info->text1);
	g_free(info->text2);
	g_free(info->text3);
	g_free(info);
	g_string_free(str,TRUE);

}

/******************************************************************************
 * Name
 *  on_autoscroll_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_autoscroll_activate(GtkMenuItem * menuitem,
				       GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set personal commentary to follow the Bible text
 *
 * Return value
 *   void
 */
 /*
    static void on_autoscroll_activate(GtkMenuItem * menuitem,
    GSHTMLEditorControlData * ecd)
    {
    settings.notefollow = GTK_CHECK_MENU_ITEM(menuitem)->active;
    }
  */
/******************************************************************************
 * Name
 *  on_editnote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_editnote_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set personal commentary to edit mode
 *
 * Return value
 *   void
 */

static void on_editnote_activate(GtkMenuItem * menuitem,
				 GSHTMLEditorControlData * ecd)
{
	if (ecd->personal_comments) {
		settings.editnote =
		    GTK_CHECK_MENU_ITEM(menuitem)->active;
		if (GTK_CHECK_MENU_ITEM(menuitem)->active) {
			gtk_widget_show(ecd->toolbar_style);
			gtk_widget_show(ecd->toolbar_edit);
		}
	}

	if (ecd->gbs) {
		settings.editgbs =
		    GTK_CHECK_MENU_ITEM(menuitem)->active;

		if (GTK_CHECK_MENU_ITEM(menuitem)->active) {
			gtk_widget_show(ecd->toolbar_style);
			gtk_widget_show(ecd->toolbar_edit);
		}
	}
	gtk_html_set_editable(GTK_HTML(ecd->html),
			      GTK_CHECK_MENU_ITEM(menuitem)->active);
}


/******************************************************************************
 * Name
 *  gui_create_editor_popup
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   GtkWidget *gui_create_editor_popup(GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    create popup menu for editor
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_editor_popup(GSHTMLEditorControlData * ecd)
{
	GtkWidget *pmEditor;
	guint tmp_key;
	GtkWidget *separator;
	GtkWidget *file_menu;
	GtkWidget *save_note = NULL;
	GtkWidget *export_note = NULL;
	GtkWidget *delete_note = NULL;
	GtkWidget *new = NULL;
	GtkWidget *open = NULL;
	GtkWidget *save = NULL;
	GtkWidget *saveas = NULL;
	GtkWidget *export_plain = NULL;
	GtkWidget *print;
	GtkWidget *edit2_menu;
	GtkWidget *cut;
	GtkWidget *copy;
	GtkWidget *paste;
	GtkWidget *spell;
	GtkWidget *undo;
	GtkWidget *find;
	GtkWidget *replace;

	ecd->editnote = NULL;

	pmEditor = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmEditor), "pmEditor", pmEditor);
	

	ecd->file = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->file)->child),
				  _("File"));
	gtk_widget_show(ecd->file);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->file);

	file_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->file), file_menu);

	new = gtk_menu_item_new_with_label(_("New"));
	gtk_widget_show(new);
	gtk_container_add(GTK_CONTAINER(file_menu), new);

	open = gtk_menu_item_new_with_label(_("Open"));
	gtk_widget_show(open);
	gtk_container_add(GTK_CONTAINER(file_menu), open);

	save = gtk_menu_item_new_with_label(_("Save"));
	gtk_widget_show(save);
	gtk_container_add(GTK_CONTAINER(file_menu), save);


	saveas = gtk_menu_item_new_with_label(_("Save As ..."));
	gtk_widget_show(saveas);
	gtk_container_add(GTK_CONTAINER(file_menu), saveas);
	
	export_plain =
	    gtk_menu_item_new_with_label(_("Export"));
	gtk_widget_show(export_plain);
	gtk_container_add(GTK_CONTAINER(file_menu), export_plain);

	

	print = gtk_menu_item_new_with_label(_("Print"));
	gtk_widget_show(print);
	gtk_container_add(GTK_CONTAINER(file_menu), print);


	ecd->edit2 = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->edit2)->child),
				  _("_Edit"));
	gtk_widget_show(ecd->edit2);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->edit2);

	edit2_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->edit2),
				  edit2_menu);

	cut = gtk_menu_item_new_with_label(_("Cut"));
	gtk_widget_show(cut);
	gtk_container_add(GTK_CONTAINER(edit2_menu), cut);

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(edit2_menu), copy);

	paste = gtk_menu_item_new_with_label(_("Paste"));
	gtk_widget_show(paste);
	gtk_container_add(GTK_CONTAINER(edit2_menu), paste);

	spell = gtk_menu_item_new_with_label(_("Spell Check"));
	gtk_widget_show(spell);
	gtk_container_add(GTK_CONTAINER(edit2_menu), spell);

#ifdef USE_SPELL
	gtk_widget_set_sensitive(spell, 1);
#else
	gtk_widget_set_sensitive(spell, 0);
#endif


	undo = gtk_menu_item_new_with_label(_("Undo"));
	gtk_widget_show(undo);
	gtk_container_add(GTK_CONTAINER(edit2_menu), undo);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(edit2_menu), find);


	replace = gtk_menu_item_new_with_label(_("Replace"));
	gtk_widget_show(replace);
	gtk_container_add(GTK_CONTAINER(edit2_menu), replace);

	ecd->link = gtk_menu_item_new_with_label(_("Link..."));
	gtk_widget_show(ecd->link);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->link);

	g_signal_connect(GTK_OBJECT(new), "activate",
			   G_CALLBACK(gui_new_activate),
			   ecd);
	g_signal_connect(GTK_OBJECT(open), "activate",
			   G_CALLBACK(on_open_activate),
			   ecd);
	g_signal_connect(GTK_OBJECT(save), "activate",
			   G_CALLBACK(on_save_activate),
			   ecd);
	g_signal_connect(GTK_OBJECT(saveas), "activate",
			   G_CALLBACK(on_save_as_activate),
			   ecd);
	g_signal_connect(GTK_OBJECT(export_plain), "activate",
			   G_CALLBACK(on_export_plain_activate),
			   ecd);
	
	g_signal_connect(GTK_OBJECT(print), "activate",
			   G_CALLBACK(on_print_activate), ecd);
	g_signal_connect(GTK_OBJECT(cut), "activate",
			   G_CALLBACK(on_cut_activate), ecd);
	g_signal_connect(GTK_OBJECT(copy), "activate",
			   G_CALLBACK(on_copy_activate), ecd);
	g_signal_connect(GTK_OBJECT(paste), "activate",
			   G_CALLBACK(on_paste_activate), ecd);

#ifdef USE_SPELL
/*	g_signal_connect(GTK_OBJECT(spell), "activate",
			   G_CALLBACK(spell_check_cb), ecd);*/
#endif	/* USE_SPELL */
	g_signal_connect(GTK_OBJECT(undo), "activate",
			   G_CALLBACK(on_undo_activate), ecd);
	g_signal_connect(GTK_OBJECT(find), "activate",
			   G_CALLBACK(on_find_activate), ecd);
	g_signal_connect(GTK_OBJECT(replace), "activate",
			   G_CALLBACK(on_replace_activate), ecd);

	g_signal_connect(GTK_OBJECT(ecd->link), "activate",
			   G_CALLBACK(on_link_activate), ecd);
	return pmEditor;
}
#endif


/******   end of file   ******/