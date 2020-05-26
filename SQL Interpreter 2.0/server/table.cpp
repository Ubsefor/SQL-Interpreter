    //
    //  table.cpp
    //  table
    //
    //  Edited by Rave Botovski on 21.04.2020.
    //  Copyright © 2020 Rave Botovski. All rights reserved.
    //

#include "Libraries/table.hpp"

    // #define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */

extern int errno;

/* FILE MAGIC - this text is written to the beginning of data file */

static char FILE_SIGNATURE[] = "\033\032DataFile\033\032~~~";

/* ****************************************************** */
/*           table file header structure                  */
/* |-------------|------------|-------|.....|-------|      */
/*   FILE_SIGNAT    TableInfo    Fld1          FldN          */
/* ****************************************************** */

typedef struct
{
    char           fieldName[MaxFieldNameLen];
    enum FieldType type;
    long           len;
    char           *pNewValue;
    char           *pEditValue;
} FieldStruct;

struct Table
{
    int         fd;
    FieldStruct *pFieldStruct;
    long        currentPos;
    Bool        editFlag;
};

struct TableInfo
{
    long dataOffset;
    long fieldNumber;       /* duplicates pFieldStruct->numOfFields */
    long recordSize;
    long totalRecordNumber; /* including deleted records */
    long recordNumber;
    long firstRecordOffset;
    long lastRecordOffset;
    long firstDeletedOffset;
} tableInfo;

struct Links
{
    long prevOffset = 0;
    long nextOffset = 0;
} Links;

/* Possible errors */
const char *ErrorText[] =
{ "Success.",
    "Can't create table.",
    "Can't open table.",
    "Field not found.",
    "Bad table handle",
    "Wrong arguments",
    "Can't set file position",
    "File write error",
    "File read error",
    "Table data corrupted",
    "Can't create table handle",
    "Can't delete or open read-only file",
    "Illegal file name",
    "Can't delete table",
    "File data corrupted",
    "Bad File Position",
    "Wrong field type",
    "Text value exceeds field length",
    "Current record is not edited", };


/*  static function prototypes */

enum Direction
{
    LINK_PREV,
    LINK_NEXT
};

static enum Errors GetCurrentRecord( struct Table *tabHandle );
static enum Errors PutCurrentRecord( struct Table *tabHandle );
static enum Errors PutNewRecord( struct Table *tabHandle,
                                long position,
                                struct Links *Links );
static enum Errors ModifyLinks( struct Table  *tabHandle,
                               long position,
                               long value,
                               enum Direction dir );
static enum Errors   GetInsertPos( struct Table *tabHandle,
                                  long         *position );
static enum Errors   GetLinks( struct Table *tabHandle,
                              long position,
                              struct Links *Links );
static enum Errors   WriteHeader( struct Table *tabHandle );
static enum Errors   ReadHeader( struct Table *tabHandle );
static struct Table* CreateTableHandle( struct TableStruct *tableStruct );
static struct Table* AllocateTableHandle( void );
static void          DeleteTableHandle( struct Table *tabHandle );
static enum Errors   AllocateBuffers( struct Table *tabHandle );
static void          DeallocateBuffers( struct Table *tabHandle );
static enum Errors   AddRecToDeleted( struct Table *tabHandle );

/*  library function definitions */

enum Errors createTable( const char *tableName, struct TableStruct *tableStruct )
{
    int          fd;
    enum Errors  retval;
    struct Table *tabHandle;
    
    if ( !tableName || !tableStruct )
        return BadArgs;
    
    if ( access( tableName, F_OK ) == 0 )
    {
        return BadFileName;
    }
    
    fd = open( tableName, CREATE_FLAGS, PERMISSION_FLAGS );
    
    if ( fd < 0 )
        switch ( errno )
        {
            case EACCES: return ReadOnlyFile;
                
            case ENOENT: return BadFileName;
                
            default:
                return CantCreateTable;
        }
    
    if ( ( tableStruct->numOfFields <= 0 ) ||
        !tableStruct->fieldsDef )
        return CorruptedData;
    
    tabHandle = CreateTableHandle( tableStruct );
    
    if ( !tabHandle )
    {
        close( fd );
        return CantCreateHandle;
    }
    
    tabHandle->fd = fd;
    retval        = WriteHeader( tabHandle );
    
    DeleteTableHandle( tabHandle );
    
    return retval;
}

enum Errors deleteTable( const char *tableName )
{
    if ( !tableName )
        return BadArgs;
    
    if ( !unlink( tableName ) )
        return OK;
    
    switch ( errno )
    {
        case EACCES: return ReadOnlyFile;
            
        case ENOENT: return BadFileName;
            
        default: return CantDeleteTable;
    }
}

enum Errors openTable( const char *tableName, THandle *tableHandle )
{
    int          fd;
    enum Errors  retval;
    struct Table *tabHandle;
    
    if ( !tableName || !tableHandle )
        return BadArgs;
    
    *tableHandle = NULL;
    
    fd = open( tableName, OPEN_FLAGS );
    
    if ( fd < 0 )
        switch ( errno )
        {
            case EACCES: return ReadOnlyFile;
                
            case ENOENT: return BadFileName;
                
            default:
                return CantOpenTable;
        }
    tabHandle = AllocateTableHandle();
    
    if ( !tabHandle )
    {
        close( fd );
        return CantCreateHandle;
    }
    
    tabHandle->fd = fd;
    retval        = ReadHeader( tabHandle );
    
    if ( retval != OK )
    {
        DeleteTableHandle( tabHandle );
        return retval;
    }
    
    retval = AllocateBuffers( tabHandle );
    
    if ( retval != OK )
    {
        DeleteTableHandle( tabHandle );
        return retval;
    }
    
    tabHandle->currentPos = tableInfo.firstRecordOffset;
    
    if ( tabHandle->currentPos >= tableInfo.dataOffset )
    {
        retval = GetCurrentRecord( tabHandle );
        
        if ( retval != OK )
        {
            DeleteTableHandle( tabHandle );
            return retval;
        }
    }
    
    *tableHandle = tabHandle;
    return OK;
}

enum Errors closeTable( THandle tableHandle )
{
    enum Errors retval = OK;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( tableHandle->fd >= 0 )
    {
        retval = WriteHeader( tableHandle );
    }
    DeleteTableHandle( tableHandle );
    return retval;
}

enum Errors moveFirst( THandle tableHandle )
{
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    tableHandle->currentPos = tableInfo.firstRecordOffset;
    
    if ( tableInfo.recordNumber == 0 )
        return OK;
    
    return GetCurrentRecord( tableHandle );
}

enum Errors moveLast( THandle tableHandle )
{
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    tableHandle->currentPos = tableInfo.lastRecordOffset;
    
    if ( tableInfo.recordNumber == 0 )
        return OK;
    
    return GetCurrentRecord( tableHandle );
}

enum Errors moveNext( THandle tableHandle )
{
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    switch ( tableHandle->currentPos )
    {
        case -1:                 /* beforeFirst */
            tableHandle->currentPos = tableInfo.firstRecordOffset;
            return GetCurrentRecord( tableHandle );
            
        case 0:                 /* after last */
            return BadPos;
            
        default:
            tableHandle->currentPos = Links.nextOffset;
            
            if ( tableHandle->currentPos >= tableInfo.dataOffset )
            {
                return GetCurrentRecord( tableHandle );
            }
            else
                return OK;
    }
}

enum Errors movePrevios( THandle tableHandle )
{
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    switch ( tableHandle->currentPos )
    {
        case -1:                 /* beforeFirst */
            return BadPos;
            
        case 0:                  /* after last */
            tableHandle->currentPos = tableInfo.lastRecordOffset;
            return GetCurrentRecord( tableHandle );
            
        default:
            tableHandle->currentPos = Links.prevOffset;
            
            if ( tableHandle->currentPos >= tableInfo.dataOffset )
            {
                return GetCurrentRecord( tableHandle );
            }
            else
                return OK;
    }
}

Bool beforeFirst( THandle tableHandle )
{
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return FALSE;
    
    switch ( tableHandle->currentPos )
    {
        case -1:                 /* beforeFirst */
            return TRUE;
            
        case 0:                  /* after last */
            return tableInfo.recordNumber == 0;
            
        default:
            return FALSE;
    }
}

Bool afterLast( THandle tableHandle )
{
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return FALSE;
    
    switch ( tableHandle->currentPos )
    {
        case -1:                 /* beforeFirst */
            return tableInfo.recordNumber == 0;
            
        case 0:                  /* after last */
            return TRUE;
            
        default:
            return FALSE;
    }
}

enum Errors getText( THandle tableHandle, const char *fieldName, char **pvalue )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName || !pvalue )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            if ( tableHandle->pFieldStruct[i].type != Text )
                return BadFieldType;
            
            *pvalue = tableHandle->pFieldStruct[i].pEditValue;
            return OK;
        }
    return FieldNotFound;
}

enum Errors getLong( THandle tableHandle, const char *fieldName, long *pvalue )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName || !pvalue )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            if ( tableHandle->pFieldStruct[i].type != Long )
                return BadFieldType;
            
            memcpy( pvalue, tableHandle->pFieldStruct[i].pEditValue, sizeof ( long ) );
            return OK;
        }
    return FieldNotFound;
}

enum Errors startEdit( THandle tableHandle )
{
    if ( !tableHandle )
        return BadHandle;
    
    if ( tableHandle->currentPos < tableInfo.dataOffset )
        return BadPos;
    
    tableHandle->editFlag = TRUE;
    return OK;
}

enum Errors putText( THandle tableHandle, const char *fieldName, const char *value )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName || !value )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            if ( tableHandle->pFieldStruct[i].type != Text )
                return BadFieldType;
            
            if ( strlen( value ) >=
                (unsigned) tableHandle->pFieldStruct[i].len )
                return BadFieldLen;
            
            strcpy( tableHandle->pFieldStruct[i].pEditValue, value );
            return OK;
        }
    return FieldNotFound;
}

enum Errors putLong( THandle tableHandle, const char *fieldName, long value )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            if ( tableHandle->pFieldStruct[i].type != Long )
                return BadFieldType;
            
            memcpy( tableHandle->pFieldStruct[i].pEditValue, &value, sizeof ( long ) );
            return OK;
        }
    return FieldNotFound;
}

enum Errors finishEdit( THandle tableHandle )
{
    if ( !tableHandle )
        return BadHandle;
    
    if ( !tableHandle->editFlag )
        return NoEditing;
    
    tableHandle->editFlag = FALSE;
    return PutCurrentRecord( tableHandle );
}

enum Errors createNew( THandle tableHandle )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
    {
        memset( tableHandle->pFieldStruct[i].pNewValue,
               0,
               tableHandle->pFieldStruct[i].len );
    }
    return OK;
}

enum Errors putTextNew( THandle tableHandle, const char *fieldName, const char *value )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName || !value )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            if ( tableHandle->pFieldStruct[i].type != Text )
                return BadFieldType;
            
            if ( strlen( value ) >=
                (unsigned) tableHandle->pFieldStruct[i].len )
                return BadFieldLen;
            
            strcpy( tableHandle->pFieldStruct[i].pNewValue, value );
            return OK;
        }
    return FieldNotFound;
}

enum Errors putLongNew( THandle tableHandle, const char *fieldName, long value )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            if ( tableHandle->pFieldStruct[i].type != Long )
                return BadFieldType;
            
            memcpy( tableHandle->pFieldStruct[i].pNewValue, &value, sizeof ( long ) );
            return OK;
        }
    return FieldNotFound;
}

enum Errors insertNew( THandle tableHandle )
{
    long         position;
    struct Links Links;
    enum Errors  retval;
    
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    retval = GetInsertPos( tableHandle, &position );
    
    if ( retval != OK )
        return retval;
    
    retval = GetLinks( tableHandle, Links.prevOffset, &Links );
    
    if ( retval != OK )
        return retval;
    
    Links.nextOffset = tableHandle->currentPos;
    
    if ( Links.nextOffset == -1 )
        Links.nextOffset = 0;
    retval = PutNewRecord( tableHandle, position, &Links );
    
    if ( retval != OK )
        return retval;
    
    retval = ModifyLinks( tableHandle, tableHandle->currentPos, position, LINK_PREV );
    
    if ( retval != OK )
        return retval;
    
    retval = ModifyLinks( tableHandle, Links.prevOffset, position, LINK_NEXT );
    
    if ( retval != OK )
        return retval;
    
    if ( tableInfo.recordNumber == 0 )
    {
        tableInfo.firstRecordOffset = position;
        tableInfo.lastRecordOffset  = position;
    }
    else if ( Links.prevOffset == -1 )
    {
        tableInfo.firstRecordOffset = position;
    }
    tableInfo.recordNumber++;
    
    return OK;
}

enum Errors insertaNew( THandle tableHandle )
{
    long         position;
    struct Links Links;
    enum Errors  retval;
    
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    retval = GetInsertPos( tableHandle, &position );
    
    if ( retval != OK )
        return retval;
    
    Links.nextOffset = tableInfo.firstRecordOffset;
    
    if ( Links.nextOffset == -1 )
        Links.nextOffset = 0;
    Links.prevOffset = -1;
    retval           = PutNewRecord( tableHandle, position, &Links );
    
    if ( retval != OK )
        return retval;
    
    retval = ModifyLinks( tableHandle,
                         tableInfo.firstRecordOffset,
                         position,
                         LINK_PREV );
    
    if ( retval != OK )
        return retval;
    
    if ( tableInfo.recordNumber == 0 )
    {
        tableInfo.lastRecordOffset = position;
    }
    tableInfo.recordNumber++;
    tableInfo.firstRecordOffset = position;
    
    
    return OK;
}

enum Errors insertzNew( THandle tableHandle )
{
    long         position;
    struct Links links;
    enum Errors  retval;
    
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    retval = GetInsertPos( tableHandle, &position );
    
    if ( retval != OK )
        return retval;
    
    /* insert at the end */
    Links.nextOffset = 0;
    Links.prevOffset = tableInfo.lastRecordOffset;
    
    if ( Links.prevOffset == 0 )
        Links.prevOffset = -1;
    
    retval = PutNewRecord( tableHandle, position, &Links );
    
    if ( retval != OK )
        return retval;
    
    retval = ModifyLinks( tableHandle,
                         tableInfo.lastRecordOffset,
                         position,
                         LINK_NEXT );
    
    if ( retval != OK )
        return retval;
    
    if ( tableInfo.recordNumber == 0 )
        tableInfo.firstRecordOffset = position;
    
    tableInfo.recordNumber++;
    tableInfo.lastRecordOffset = position;
    return OK;
}

enum Errors getFieldLen( THandle tableHandle, const char *fieldName, unsigned *plen )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName || !plen )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            *plen = tableHandle->pFieldStruct[i].len;
            
            if ( tableHandle->pFieldStruct[i].type == Text )
                *plen = *plen - 1;
            return OK;
        }
    return FieldNotFound;
}

enum Errors getFieldType( THandle tableHandle, const char *fieldName, enum FieldType *ptype )
{
    int i;
    
    if ( !tableHandle )
        return BadHandle;
    
    if ( !fieldName || !ptype )
        return BadArgs;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
        if ( strcmp( fieldName, tableHandle->pFieldStruct[i].fieldName ) == 0 )
        {
            *ptype = tableHandle->pFieldStruct[i].type;
            return OK;
        }
    return FieldNotFound;
}

enum Errors getFieldsNum( THandle tableHandle, unsigned *pNum )
{
    if ( !tableHandle )
        return BadHandle;
    
    if ( !pNum )
        return BadArgs;
    
    *pNum = tableInfo.fieldNumber;
    return OK;
}

enum Errors getFieldName( THandle tableHandle, unsigned index, char **pFieldName )
{
    if ( !tableHandle )
        return BadHandle;
    
    if ( !pFieldName )
        return BadArgs;
    
    *pFieldName = tableHandle->pFieldStruct[index].fieldName;
    return OK;
}

enum Errors deleteRec( THandle tableHandle )
{
    if ( !tableHandle || ( tableHandle->fd < 0 ) )
        return BadHandle;
    
    if ( tableHandle->currentPos < tableInfo.firstRecordOffset )
        return BadPosition;
    
    return AddRecToDeleted( tableHandle );
}

/*  internal function definitions */

/* helper macros */
#define MOVE_POS( pos ) \
if ( lseek( tabHandle->fd, ( pos ), SEEK_SET ) != (int) ( pos ) ) return CantMoveToPos;

#define WRITE_DATA( buf, size ) \
if ( write( tabHandle->fd, ( buf ), ( size ) ) != (int) ( size ) ) return CantWriteData;

#define READ_DATA( buf, size ) \
if ( read( tabHandle->fd, ( buf ), ( size ) ) != (int) ( size ) ) return CantReadData;

static enum Errors WriteHeader( struct Table *tabHandle )
{
    MOVE_POS( 0 );
    WRITE_DATA( FILE_SIGNATURE, sizeof ( FILE_SIGNATURE ) );
    WRITE_DATA( &tableInfo, sizeof ( struct TableInfo ) );
    
    if ( tabHandle->pFieldStruct )
    {
        WRITE_DATA( tabHandle->pFieldStruct,
                   sizeof ( FieldStruct ) * tableInfo.fieldNumber );
    }
    else
        return CantWriteData;
    
    return OK;
}

static struct Table* AllocateTableHandle()
{
    struct Table *handle;
    
    handle = (struct Table *) calloc( 1, sizeof ( struct Table ) );
    
    if ( !handle )
        return NULL;
    
    handle->fd           = -1;
    handle->pFieldStruct = NULL;
    handle->currentPos   = -1;
    
    tableInfo.recordNumber       = 0;
    tableInfo.totalRecordNumber  = 0;
    tableInfo.firstRecordOffset  = -1;
    tableInfo.firstDeletedOffset = -1;
    tableInfo.lastRecordOffset   = 0;
    tableInfo.fieldNumber        = 0;
    tableInfo.recordSize         = 0;
    tableInfo.dataOffset         = 0;
    Links.prevOffset             = -1;
    Links.nextOffset             = 0;
    handle->editFlag             = FALSE;
    return handle;
}

static struct Table* CreateTableHandle( struct TableStruct *tableStruct )
{
    struct Table *handle;
    unsigned     i;
    long         RecSize = 0;
    
    handle = AllocateTableHandle();
    
    if ( !handle )
        return NULL;
    
    /* copy structure and count record size */
    tableInfo.fieldNumber = tableStruct->numOfFields;
    handle->pFieldStruct  =
    (FieldStruct *) calloc( tableStruct->numOfFields, sizeof ( FieldStruct ) );
    
    if ( !handle->pFieldStruct )
    {
        DeleteTableHandle( handle );
        return NULL;
    }
    
    for ( i = 0; i < tableStruct->numOfFields; i++ )
    {
        strcpy( handle->pFieldStruct[i].fieldName, tableStruct->fieldsDef[i].name );
        handle->pFieldStruct[i].type = tableStruct->fieldsDef[i].type;
        
        switch ( tableStruct->fieldsDef[i].type )
        {
            case Long:
                handle->pFieldStruct[i].len = sizeof ( long );
                RecSize += sizeof ( long );
                break;
                
            case Text:
                
                if ( !tableStruct->fieldsDef[i].len )
                {
                    DeleteTableHandle( handle );
                    return NULL;
                }
                handle->pFieldStruct[i].len = tableStruct->fieldsDef[i].len + 1;
                RecSize += tableStruct->fieldsDef[i].len + 1;
                break;
                
            default:
                break;
        }
    }
    
    tableInfo.recordSize = RecSize;
    tableInfo.dataOffset = sizeof ( FILE_SIGNATURE ) +
    sizeof ( struct TableInfo ) + tableInfo.fieldNumber *
    sizeof ( FieldStruct );
    
    return handle;
}

static void DeleteTableHandle( struct Table *tabHandle )
{
    if ( tabHandle->fd >= 0 )
        close( tabHandle->fd );
    DeallocateBuffers( tabHandle );
    free( tabHandle->pFieldStruct );
    free( tabHandle );
}

static enum Errors AllocateBuffers( struct Table *tabHandle )
{
    int i;
    
    if ( !tabHandle->pFieldStruct )
        return BadHandle;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
    {
        tabHandle->pFieldStruct[i].pNewValue = (char *) malloc(
                                                               tabHandle->pFieldStruct[i].len );
        tabHandle->pFieldStruct[i].pEditValue = (char *) malloc(
                                                                tabHandle->pFieldStruct[i].len );
        
        if ( !tabHandle->pFieldStruct[i].pNewValue ||
            !tabHandle->pFieldStruct[i].pEditValue )
            return CantCreateHandle;
    }
    return OK;
}

static void DeallocateBuffers( struct Table *tabHandle )
{
    int i;
    
    if ( !tabHandle->pFieldStruct )
        return;
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
    {
        if ( tabHandle->pFieldStruct[i].pNewValue )
            free(
                 tabHandle->pFieldStruct[i].pNewValue );
        
        if ( tabHandle->pFieldStruct[i].pEditValue )
            free(
                 tabHandle->pFieldStruct[i].pEditValue );
    }
}

static enum Errors ReadHeader( struct Table *tabHandle )
{
    char FileSig[sizeof ( FILE_SIGNATURE )];
    
    MOVE_POS( 0 );
    READ_DATA( FileSig, sizeof ( FILE_SIGNATURE ) );
    
    if ( strcmp( FileSig, FILE_SIGNATURE ) )
        return CorruptedFile;
    
    READ_DATA( &tableInfo, sizeof ( struct TableInfo ) );
    
    if ( tableInfo.fieldNumber <= 0 )
        return CorruptedFile;
    
    tabHandle->pFieldStruct =
    (FieldStruct *) calloc( tableInfo.fieldNumber, sizeof ( FieldStruct ) );
    
    if ( !tabHandle->pFieldStruct )
        return CantReadData;
    
    READ_DATA( tabHandle->pFieldStruct, sizeof ( FieldStruct ) * tableInfo.fieldNumber );
    return OK;
}

static enum Errors GetCurrentRecord( struct Table *tabHandle )
{
    int i;
    
    if ( tabHandle->currentPos < tableInfo.dataOffset )
        return BadPos;
    
    MOVE_POS( tabHandle->currentPos );
    READ_DATA( &Links, sizeof ( Links ) );
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
    {
        READ_DATA( tabHandle->pFieldStruct[i].pEditValue,
                  tabHandle->pFieldStruct[i].len );
    }
    return OK;
}

static enum Errors GetInsertPos( struct Table *tabHandle, long *position )
{
    struct Links Links;
    
    if ( tableInfo.firstDeletedOffset >= tableInfo.dataOffset )
    {
        *position = tableInfo.firstDeletedOffset;
        MOVE_POS( *position );
        READ_DATA( &Links, sizeof ( struct Links ) );
        
        if ( Links.prevOffset != -1 )
            return CorruptedFile;
        
        tableInfo.firstDeletedOffset = Links.nextOffset;
    }
    else
    {
        /* no deleted records - position at the end of file */
        *position = tableInfo.dataOffset + tableInfo.totalRecordNumber *
        ( tableInfo.recordSize + sizeof ( struct Links ) );
        MOVE_POS( *position );
        tableInfo.totalRecordNumber++;
    }
    return OK;
}

static enum Errors GetLinks( struct Table *tabHandle, long position, struct Links *Links )
{
    if ( ( position == 0 ) || ( position == -1 ) )
    {
        Links->prevOffset = -1;
        Links->nextOffset = 0;
    }
    else
    {
        MOVE_POS( position );
        READ_DATA( Links, sizeof ( struct Links ) );
    }
    return OK;
}

static enum Errors ModifyLinks( struct Table  *tabHandle, long position, long value, enum Direction dir )
{
    long posToWrite = 0;
    
    if ( ( position != 0 ) && ( position != -1 ) )
    {
        switch ( dir )
        {
            case LINK_PREV: posToWrite = position;
                break;
                
            case LINK_NEXT: posToWrite = position + sizeof ( long );
                break;
        }
        MOVE_POS( posToWrite );
        WRITE_DATA( &value, sizeof ( value ) );
    }
    return OK;
}

static enum Errors PutNewRecord( struct Table *tabHandle, long position, struct Links *Links )
{
    int i;
    
    MOVE_POS( position );
    WRITE_DATA( Links, sizeof ( struct Links ) );
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
    {
        WRITE_DATA( tabHandle->pFieldStruct[i].pNewValue,
                   tabHandle->pFieldStruct[i].len );
    }
    return OK;
}

static enum Errors PutCurrentRecord( struct Table *tabHandle )
{
    int i;
    
    MOVE_POS( tabHandle->currentPos );
    WRITE_DATA( &Links, sizeof ( struct Links ) );
    
    for ( i = 0; i < tableInfo.fieldNumber; i++ )
    {
        WRITE_DATA( tabHandle->pFieldStruct[i].pEditValue,
                   tabHandle->pFieldStruct[i].len );
    }
    return OK;
}

static enum Errors AddRecToDeleted( struct Table *tabHandle )
{
    static unsigned long DELETE_MARK = ~0;
    
        // insert record to the deleted list
    MOVE_POS( tabHandle->currentPos );
    WRITE_DATA( &DELETE_MARK, sizeof DELETE_MARK );
    WRITE_DATA( &tableInfo.firstDeletedOffset, sizeof ( tableInfo.firstDeletedOffset ) );
    tableInfo.firstDeletedOffset = tabHandle->currentPos;
    tableInfo.recordNumber--;
    
        // modify prev and next Links
    if ( Links.prevOffset == -1 ) // first record is deleted
    {
        tableInfo.firstRecordOffset = Links.nextOffset;
        
        if ( tableInfo.recordNumber == 0 )
        {
            tableInfo.firstRecordOffset = -1; // no more records
            tableInfo.lastRecordOffset  = 0;  // no more records
        }
    }
    else
    {
        MOVE_POS( Links.prevOffset + sizeof ( Links.prevOffset ) );
        WRITE_DATA( &Links.nextOffset, sizeof ( Links.nextOffset ) );
    }
    
    if ( Links.nextOffset == 0 ) // last record is deleted
    {
        tableInfo.lastRecordOffset = Links.prevOffset;
        
        if ( tableInfo.recordNumber == 0 )
        {
            tableInfo.firstRecordOffset = -1; // no more records
            tableInfo.lastRecordOffset  = 0;  // no more records
        }
    }
    else
    {
        MOVE_POS( Links.nextOffset );
        WRITE_DATA( &Links.prevOffset, sizeof ( Links.prevOffset ) );
    }
    return OK;
}
