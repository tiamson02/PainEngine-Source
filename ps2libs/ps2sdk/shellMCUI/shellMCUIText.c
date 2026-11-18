//-----------------------------------------------------------------------------
//               shellMCUIText - Memory Card User Interface Driver
//
// TRC compliant message text 
// interface implementation
//
// $Workfile: shellMCUIText.c $
// $Author: Hbodasin $
// $Date: 13/12/02 12:49 $
// $Revision: 7 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Further notes, if any
//
//-----------------------------------------------------------------------------
#include "shellMCUI.h"


//To enter hard spaces in CW: Use numeric keypad & type Alt-0160 " ":
//Without hard spaces	:	memory card (8MB) (for PlayStation®2)//							
//							Memory card (8MB) (for PlayStation®2)
//							memory card (8MB) (pour PlayStation®2)
//							Memory card (8MB) (pour PlayStation®2)
//							MEMORY CARD
//
//With hard spaces		: 	memory card (8MB) (for PlayStation®2)
//							Memory card (8MB) (for PlayStation®2)
//							memory card (8MB) (pour PlayStation®2)
//							Memory card (8MB) (pour PlayStation®2)
//							MEMORY CARD
//
//Note: '®' character code is 174 (0xAE)
const char* SHELLMCUI_STATUS_TEXT_ENGLISH[SHELLMCUI_STATUS_NUM] =
{

	//SHELLMCUI_STATUS_DETECTING				//Doc ref 7.0.1
	//"Detecting Memory card (8MB)(for PlayStation®2)...",								
	"Checking memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1. Do not remove memory card (8MB) (for PlayStation®2), reset, or switch off the console",	

	//SHELLMCUI_STATUS_PS2_CARD_INSERTED		//No Doc ref 
	"Memory card (8MB) (for PlayStation®2) is inserted in MEMORY CARD slot 1",

	//SHELLMCUI_ERROR_PS2_CARD_NOT_INSERTED		//Doc ref 3.0.4	
	//"Memory card (8MB)(for PlayStation®2) is not inserted into MEMORY CARD slot 1",				
	"No memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1",								

	//SHELLMCUI_ERROR_UNFORMATTED_CARD			//Doc ref 4.0.2		
	"Memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1 is unformatted",					

	//SHELLMCUI_ERROR_INSUFFICIENT_SPACE		//Doc ref 3.0.2	
	"Insufficient space on memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1",			
	//(xxx) requires (x)KB of free space to save data.

	//SHELLMCUI_ERROR_NO_SAVES					//Doc ref 4.0.1	
	//"No (xxx) save data present on memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1",
	"No save data for this title present on memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1",	
	
	//SHELLMCUI_STATUS_REQUEST_USER_FORMAT,		//Doc ref 3.0.1 (in conjunction with SHELLMCUI_ERROR_UNFORMATTED_CARD)
	"Format memory card (8MB) (for PlayStation®2)?",												
	
	//SHELLMCUI_STATUS_REQUEST_USER_OVERWRITE,	//Doc ref 3.0.7 (modified) (in conjunction with SHELLMCUI_ERROR_SAVE_EXISTS)
	"Filename already exists. Overwrite previous data?",											

	//Load / save status:
	//SHELLMCUI_STATUS_LOADING					//Doc ref 4.0.7
	"Loading data. Do not remove memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1, reset, or switch off the console",	

	//SHELLMCUI_STATUS_SAVING,					//Doc ref 3.1.0	
	"Saving data. Do not remove memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1, reset, or switch off the console",	

	//SHELLMCUI_STATUS_FORMATTING				//Doc ref 8.0.3
	"Formatting memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1. Do not remove memory card (8MB) (for PlayStation®2), reset, or switch off console",	
	
	//SHELLMCUI_STATUS_LOAD_SUCCESSFUL			//Doc ref 4.0.8
	"Load Successful", 																				

	//SHELLMCUI_STATUS_SAVE_SUCCESSFUL			//Doc ref 3.1.3
	"Save Successful", 																				

	//SHELLMCUI_STATUS_FORMAT_SUCCESSFUL,		//Doc ref 8.0.6 (Unused since proceed straight to save following format)
	"Format Successful", 																			

	//SHELLMCUI_STATUS_LIST_SAVES_SUCCESSFUL	//No Doc ref 
	//List save status:
	"", 																							

	//SHELLMCUI_ERROR_LOAD_FAILED				//Doc ref 4.0.5
	"Load failed! Check memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1 and please try again",	

	 //SHELLMCUI_ERROR_SAVE_FAILED				//Doc ref 3.0.8
	"Save failed! Check memory card (8MB) (for PlayStation®2) in MEMORY CARD slot 1 and please try again",

	//SHELLMCUI_ERROR_FORMAT_FAILED				//Doc ref 8.0.7
	"Format failed! Please check memory card (8MB) (for PlayStation®2) and try again"						
};



const char* SHELLMCUI_STATUS_TEXT_FRENCH[SHELLMCUI_STATUS_NUM] = 
{
	//SHELLMCUI_STATUS_DETECTING				//Doc ref 7.0.1
	"Vérification de la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1. Ne pas retirer la memory card (8MB) (pour PlayStation®2), ni appuyer sur reset ou éteindre la console",

	//SHELLMCUI_STATUS_PS2_CARD_INSERTED		//No Doc ref 
	"Memory card (8MB) (pour PlayStation®2) est dans la fente pour MEMORY CARD 1",						

	//SHELLMCUI_ERROR_PS2_CARD_NOT_INSERTED		//Doc ref 3.0.4	
	"Pas de memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1",						

	//SHELLMCUI_ERROR_UNFORMATTED_CARD			//Doc ref 4.0.2		
	"La memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1 n'est pas formatée",	

	//SHELLMCUI_ERROR_INSUFFICIENT_SPACE		//Doc ref 3.0.2	
	"Espace insuffisant sur la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1.",
	//"(xxx) nécessite (x) KB d’espace libre pour sauvegarder", 	

	//SHELLMCUI_ERROR_NO_SAVES					//Doc ref 4.0.1 (modified by Lionel)
	"Pas de données sauvegardées pour ce jeu sur la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1",								

	//SHELLMCUI_STATUS_REQUEST_USER_FORMAT,		//Doc ref 3.0.1 (in conjunction with SHELLMCUI_ERROR_UNFORMATTED_CARD)
	"Formater la memory card (8MB) (pour PlayStation®2) ?",										

	//SHELLMCUI_STATUS_REQUEST_USER_OVERWRITE,	//Doc ref 3.0.7 (modified) (in conjunction with SHELLMCUI_ERROR_SAVE_EXISTS)
	"Le fichier existe déjà. Voulez-vous l'écraser ?",												

	//SHELLMCUI_STATUS_LOADING					//Doc ref 4.0.7
	"Chargement en cours. Ne pas retirer la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1, ni appuyer sur reset ou éteindre la console",		

	//SHELLMCUI_STATUS_SAVING,					//Doc ref 3.1.0	
	"Sauvegarde en cours. Ne pas retirer la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1, ni appuyer sur reset ou éteindre la console",		

	//SHELLMCUI_STATUS_FORMATTING				//Doc ref 8.0.3
	"Formatage de la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1. Ne pas retirer la memory card (8MB) (pour PlayStation®2), ni appuyer sur reset ou éteindre la console",	

	//SHELLMCUI_STATUS_LOAD_SUCCESSFUL			//Doc ref 4.0.8
	"Chargement réussi",																			

	//SHELLMCUI_STATUS_SAVE_SUCCESSFUL			//Doc ref 3.1.3
	"Sauvegarde réussie",																			

	//SHELLMCUI_STATUS_FORMAT_SUCCESSFUL,		//Doc ref 8.0.6 (Unused since proceed straight to save following format)
	"Formatage réussi",																				

	//SHELLMCUI_STATUS_LIST_SAVES_SUCCESSFUL	//No Doc ref 
	"", 																							
	
	//SHELLMCUI_ERROR_LOAD_FAILED				//Doc ref 4.0.5
	"Echec du chargement ! Vérifier la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1 et réessayer",			

	 //SHELLMCUI_ERROR_SAVE_FAILED				//Doc ref 3.0.8
	"Echec de la sauvegarde ! Vérifier la memory card (8MB) (pour PlayStation®2) dans la fente pour MEMORY CARD 1 et réessayer",		

	//SHELLMCUI_ERROR_FORMAT_FAILED				//Doc ref 8.0.7
	"Echec du formatage ! Vérifier la memory card (8MB) (pour PlayStation®2) et réessayer"												
};


const char** SHELLMCUI_STATUS_TEXT_LANGUAGES[SHELLMCUI_LANGUAGE_NUM] = 
{
	SHELLMCUI_STATUS_TEXT_ENGLISH, 
	SHELLMCUI_STATUS_TEXT_FRENCH
};



