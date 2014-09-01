//***************************************************************************
//  NARS2000 -- Syntax Object Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/


// DO NOT EDIT THIS FILE
// It is automatically generated by the APL function <synobj>
//   in J:\workspaces\2by2.ws.nars.  Instead, edit and run the APL function.

#ifdef DEFINE_VARS
  #define EXTERN
#else
  #define EXTERN extern
#endif

// Syntax Objects enum
typedef enum tagSO_ENUM
{
	soA   = 0, 			// 00:  Array                                                  
	soMF     , 			// 01:  Monadic (primitive, user-defined, train, etc.) function
	soF      , 			// 02:  Ambivalent function                                    
	soNF     , 			// 03:  Niladic function                                       
	soMOP    , 			// 04:  Monadic operator                                       
	soDOP    , 			// 05:  Dyadic operator                                        
	soMOPN   , 			// 06:  Monadic operator, niladic derived function             
	soDOPN   , 			// 07:  Dyadic operator, ...                                   
	soIDX    , 			// 08:  Indexing []                                            
	soSP     , 			// 09:  Assignment/Sink                                        
	soGO     , 			// 0A:  Goto                                                   
	soJ      , 			// 0B:  Jot                                                    
	soJD     , 			// 0C:  Jot Dot                                                
	soLP     , 			// 0D:  Left paren                                             
	soRP     , 			// 0E:  Right paren                                            
	soLBK    , 			// 0F:  Left bracket                                           
	soSEMI   , 			// 10:  Semicolon                                              
	soRBK    , 			// 11:  Right bracket                                          
	soHY     , 			// 12:  Hybrid:  /\ and barred forms                           
	soSPA    , 			// 13:  {is} A                                                 
	soSPF    , 			// 14:  {is} F                                                 
	soSPM    , 			// 15:  {is} MOP                                               
	soSPD    , 			// 16:  {is} DOP                                               
	soSPHY   , 			// 17:  {is} HY                                                
	soSPNF   , 			// 18:  {is} NF                                                
	soSPFR   , 			// 19:  {is} FR                                                
	soSPMR   , 			// 1A:  {is} MR                                                
	soSPDR   , 			// 1B:  {is} DR                                                
	soSPHR   , 			// 1C:  {is} HR                                                
	soSPMN   , 			// 1D:  {is} MOPN                                              
	soSPDN   , 			// 1E:  {is} DOPN                                              
	soFF     , 			// 1F:  F F   - Trains                                         
	soFR     , 			// 20:  F)    - ...                                            
	soMR     , 			// 21:  MOP)  - ...                                            
	soDR     , 			// 22:  DOP)  - ...                                            
	soHR     , 			// 23:  HY)   - ...                                            
	soFFR    , 			// 24:  F F)  - ...                                            
	soAFR    , 			// 25:  A F)  - ...                                            
	soNAM    , 			// 26:  Name                                                   
	soSA     , 			// 27:  Strand                                                 
	soLBC    , 			// 28:  Left brace                                             
	soRBC    , 			// 29:  Right brace                                            
	soCSF    , 			// 2A:  :FOR/:FORLCL                                           
	soCSI    , 			// 2B:  :IN                                                    
	soCSIA   , 			// 2C:  :IN A                                                  
	soCSFN   , 			// 2D:  :FOR NAM/:FORLCL NAM                                   
	soCSEF   , 			// 2E:  :ENDFOR/:ENDFORLCL                                     
	soCS0    , 			// 2F:  CS Niladic                                             
	soCS1    , 			// 30:  CS Monadic                                             
	soSOS    , 			// 31:  Start of stmt                                          
	soAFOG   , 			// 32:  AFO Guard                                              
	soAFOR   , 			// 33:  AFO Return                                             
	soSYSN   , 			// 34:  System Namespace                                       
	soARBK   , 			// 35:  A RBR                                                  
	soSRBK   , 			// 36:  ; RBR/RBR2                                             
	soIO     , 			// 37:  I/O                                                    
	soNR     , 			// 38:  NAM)                                                   
	soNNR    , 			// 39:  NAM NAM)                                               
	soLNR    , 			// 3A:  (NAM NAM)                                              
	soISPA   , 			// 3B:  IDX SPA                                                
	soSYNR   , 			// 3C:  SYNTAX ERROR                                           
	soVALR   , 			// 3D:  VALUE ERROR                                            
	soEOS    , 			// 3E:  *MUST* be the highest numbered entry                   
	soNONE = 999,		//999:  Filler for no entry
} SO_ENUM, *LPSO_ENUM;

#define soLAST		soEOS

#ifdef DEBUG
// Syntax Object Names
EXTERN
LPWCHAR soNames[]
#ifdef DEFINE_VALUES
= {
	L"A"   ,			// 00:  Array                                                  
	L"MF"  ,			// 01:  Monadic (primitive, user-defined, train, etc.) function
	L"F"   ,			// 02:  Ambivalent function                                    
	L"NF"  ,			// 03:  Niladic function                                       
	L"MOP" ,			// 04:  Monadic operator                                       
	L"DOP" ,			// 05:  Dyadic operator                                        
	L"MOPN",			// 06:  Monadic operator, niladic derived function             
	L"DOPN",			// 07:  Dyadic operator, ...                                   
	L"IDX" ,			// 08:  Indexing []                                            
	L"SP"  ,			// 09:  Assignment/Sink                                        
	L"GO"  ,			// 0A:  Goto                                                   
	L"J"   ,			// 0B:  Jot                                                    
	L"JD"  ,			// 0C:  Jot Dot                                                
	L"("   ,			// 0D:  Left paren                                             
	L")"   ,			// 0E:  Right paren                                            
	L"["   ,			// 0F:  Left bracket                                           
	L";"   ,			// 10:  Semicolon                                              
	L"]"   ,			// 11:  Right bracket                                          
	L"HY"  ,			// 12:  Hybrid:  /\ and barred forms                           
	L"SPA" ,			// 13:  {is} A                                                 
	L"SPF" ,			// 14:  {is} F                                                 
	L"SPM" ,			// 15:  {is} MOP                                               
	L"SPD" ,			// 16:  {is} DOP                                               
	L"SPHY",			// 17:  {is} HY                                                
	L"SPNF",			// 18:  {is} NF                                                
	L"SPFR",			// 19:  {is} FR                                                
	L"SPMR",			// 1A:  {is} MR                                                
	L"SPDR",			// 1B:  {is} DR                                                
	L"SPHR",			// 1C:  {is} HR                                                
	L"SPMN",			// 1D:  {is} MOPN                                              
	L"SPDN",			// 1E:  {is} DOPN                                              
	L"FF"  ,			// 1F:  F F   - Trains                                         
	L"FR"  ,			// 20:  F)    - ...                                            
	L"MR"  ,			// 21:  MOP)  - ...                                            
	L"DR"  ,			// 22:  DOP)  - ...                                            
	L"HR"  ,			// 23:  HY)   - ...                                            
	L"FFR" ,			// 24:  F F)  - ...                                            
	L"AFR" ,			// 25:  A F)  - ...                                            
	L"NAM" ,			// 26:  Name                                                   
	L"SA"  ,			// 27:  Strand                                                 
	L"{"   ,			// 28:  Left brace                                             
	L"}"   ,			// 29:  Right brace                                            
	L"CSF" ,			// 2A:  :FOR/:FORLCL                                           
	L"CSI" ,			// 2B:  :IN                                                    
	L"CSIA",			// 2C:  :IN A                                                  
	L"CSFN",			// 2D:  :FOR NAM/:FORLCL NAM                                   
	L"CSEF",			// 2E:  :ENDFOR/:ENDFORLCL                                     
	L"CS0" ,			// 2F:  CS Niladic                                             
	L"CS1" ,			// 30:  CS Monadic                                             
	L"SOS" ,			// 31:  Start of stmt                                          
	L"AFOG",			// 32:  AFO Guard                                              
	L"AFOR",			// 33:  AFO Return                                             
	L"SYSN",			// 34:  System Namespace                                       
	L"ARBK",			// 35:  A RBR                                                  
	L"SRBK",			// 36:  ; RBR/RBR2                                             
	L"IO"  ,			// 37:  I/O                                                    
	L"NR"  ,			// 38:  NAM)                                                   
	L"NNR" ,			// 39:  NAM NAM)                                               
	L"LNR" ,			// 3A:  (NAM NAM)                                              
	L"ISPA",			// 3B:  IDX SPA                                                
	L"SYNR",			// 3C:  SYNTAX ERROR                                           
	L"VALR",			// 3D:  VALUE ERROR                                            
	L"EOS" ,			// 3E:  *MUST* be the highest numbered entry                   
  }
#endif
;
#endif

#undef  EXTERN

#define SONAMES_MAXLEN	5


//***************************************************************************
//  End of File: synobj.h
//***************************************************************************