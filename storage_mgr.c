
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"

SM_FileHandle fh;

void initStorageManager(void)
{
    fh.totalNumPages = 0;
}

// Create a new page file fileName
RC createPageFile(char *fileName)
{
    FILE *fp;
    fh.totalNumPages = fh.totalNumPages + 1;
    int outbf[1] = {fh.totalNumPages};
    // The reading will start from the beginning but writing can only be appended.
    fp = fopen(fileName, "w");

    // add code is that write '/0' to newfile   
    char *block_memory = malloc(PAGE_SIZE * sizeof(char)* 2); //Reserve space for page 0
    // 2 page -OK
    memset(block_memory, '\0', PAGE_SIZE* 2); 
    fwrite(block_memory, sizeof(char), PAGE_SIZE* 2, fp);   
    // move to the top of the file
    fseek(fp, 0L, SEEK_SET);
    // write the totalnumber of pages into header
    fwrite(outbf, sizeof(int), 1, fp);
    free(block_memory);    
    fclose(fp);                                
    printf("createPageFile done \n");
    return RC_OK;
}

// Opens an existing page file
RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{
    // file exists
    if (access(fileName, F_OK) == 0)
    {
        FILE *fp;
        int inbuf[1];
        fp = fopen(fileName, "r+");
        fHandle->fileName = fileName;
        // move to the top of the file
        fseek(fp, 0L, SEEK_SET);
        // read header
        fread(inbuf, sizeof(int), 1, fp);
        // read the total number of pages
        fHandle->totalNumPages = inbuf[0];
        fHandle->curPagePos = 0;
        fHandle->mgmtInfo = fp;
        return RC_OK;
    }
    else
    {
        // file doesn't exist
        return RC_FILE_NOT_FOUND;
    }
}

// Close an open page file
RC closePageFile(SM_FileHandle *fHandle)
{
    // file exists
    if (access(fHandle->fileName, F_OK) == 0)
    {
        FILE *fp = fHandle->mgmtInfo;
        int outbf[1] = {fHandle->totalNumPages};
        // move to the top of the file
        fseek(fp, 0L, SEEK_SET);
        // write the totalnumber of pages into header
        fwrite(outbf, sizeof(int), 1, fp);
        fclose(fp);

        return RC_OK;
    }
    else
    {
        // file doesn't exist
        return RC_FILE_NOT_FOUND;
    }
}

// Destroy (delete) a page file
RC destroyPageFile(char *fileName)
{
    if (remove(fileName) == 0)
    {
        // Deleted successfully
        return RC_OK;
    }
    else
    {
        // Unable to delete the file
        return RC_FILE_NOT_FOUND;
    }
}



RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    // if fHandle is  null
    if (fHandle == NULL) {   return RC_FILE_HANDLE_NOT_INIT;}
    
    // get  file pointer , only read  
    // all operation of read block is started 0 page number ( from o page to  pageNum page ) 
    FILE *file = fHandle->mgmtInfo;
    
    if (file == NULL)
    {
        printf("can not found file");
        return RC_FILE_NOT_FOUND;
    }

    // if pageNum   great than fild page number or pageNum less than 0
    if (pageNum > (fHandle->totalNumPages) || pageNum < 1 )
    {
        return RC_READ_NON_EXISTING_PAGE;
    }

    // calculate  offset of pageNum In bytes
    int offset = pageNum*PAGE_SIZE * sizeof(char);

    //Move to file pointer to pagenum page  //  SEEK_SET is  mean  starting position 
    int  seekRight =fseek(file, offset, SEEK_SET);

    // Write pageNum page data to memPage as in memory  (PAGE_SIZE =4096)
    int readRight =fread(memPage, sizeof(char), PAGE_SIZE, file); 
	if(readRight != PAGE_SIZE){
        return RC_READ_NON_EXISTING_PAGE ;
    }
    // let current page is pageNum
	fHandle->curPagePos = pageNum;

    return RC_OK;
}

// get the position of block in process
int getBlockPos(SM_FileHandle *fHandle)  
{
    //Check whether fHandle initialized
    if (fHandle == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }else{
        return fHandle->curPagePos;
    }
}

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) 
{
   // just read  page of first data
    return readBlock(1, fHandle, memPage);
}


RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    // just read previous page of curent page
    return readBlock(fHandle->curPagePos-1, fHandle, memPage);
}

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    // just read current page number block
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    // just read next page of curent page
    return readBlock(fHandle->curPagePos+1, fHandle, memPage);
}

RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->totalNumPages - 1 , fHandle, memPage);
}


/* writing blocks to a page file */
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if (pageNum > fHandle->totalNumPages || pageNum < 1)//check the pageNum is it valid.
    {
        return RC_WRITE_FAILED;
    }
    
    FILE *f = fHandle->mgmtInfo;
    fseek(f, pageNum * PAGE_SIZE, SEEK_SET);// seek to the specific page number
    if (fwrite(memPage, PAGE_SIZE, 1, f) != 1)//write the block
    {
        return RC_WRITE_FAILED;
    }

    fHandle->curPagePos = pageNum;//Update the curPagePos
    if (pageNum == fHandle->totalNumPages)
    {
        fHandle->totalNumPages += 1;
    }
    
    return RC_OK;
}


RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}


RC appendEmptyBlock(SM_FileHandle *fHandle)
{
    
    FILE *f = fHandle->mgmtInfo;
    int end = fHandle->totalNumPages;
    fseek(f, end * PAGE_SIZE, SEEK_SET);//seek to the page
    
    char buffer[PAGE_SIZE];
    memset(buffer, '\0', sizeof(buffer));

    if (fwrite(buffer, PAGE_SIZE, 1, f) != 1)
    {
        return RC_WRITE_FAILED;
    }
    
    fHandle->curPagePos = fHandle->totalNumPages;
    fHandle->totalNumPages += 1;//increase the total page
    return RC_OK;
}


RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle)
{
    if (fHandle->totalNumPages < numberOfPages)//check the capacity
    {
        for (int i = 0; i < numberOfPages - fHandle->totalNumPages; i++)
        {
            RC code = appendEmptyBlock(fHandle);//append an empty block
            if (code != RC_OK) return code;
        }
    }
    return RC_OK;
}
