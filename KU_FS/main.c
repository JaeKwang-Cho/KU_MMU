#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#define BLOCK_SIZE (4096)
#define BIT8(n)  unsgined char b##n


typedef union fileEntry {
    int val;
    struct {
        char inum;
        char Name[3];
    };
} fileEntry;

typedef union byteLine {
    unsigned char val;
    struct {
        unsigned char h0: 4;
        unsigned char h1: 4;
    };
    struct {
        unsigned char b0: 1;
        unsigned char b1: 1;
        unsigned char b2: 1;
        unsigned char b3: 1;
        unsigned char b4: 1;
        unsigned char b5: 1;
        unsigned char b6: 1;
        unsigned char b7: 1;
    };
} byteLine;

typedef union inode {
    unsigned char val[256];
    struct {
        unsigned int fsize;
        unsigned int blocks;
        unsigned int pointer[12];
    };
} inode;

typedef struct block {
    byteLine byteLines[BLOCK_SIZE];
} block;

typedef struct root_block {
    fileEntry entries[1024];
} root_block;


block *super_blocks;
block *i_bmap_blocks;
block *d_bmap_blocks;
inode *inode_blocks;
block *data_blocks;

root_block rootBlock;

unsigned int usedDataBlock = 56;
unsigned int rootOffset = 1;

void initPartition() {
    super_blocks = (block *) malloc(sizeof(block));
    i_bmap_blocks = (block *) malloc(sizeof(block));
    d_bmap_blocks = (block *) malloc(sizeof(block));
    inode_blocks = (inode *) malloc(sizeof(block) * 5);
    data_blocks = (block *) malloc(sizeof(block) * 56);

    memset(super_blocks, 0, sizeof(block));
    memset(i_bmap_blocks, 0, sizeof(block));
    memset(d_bmap_blocks, 0, sizeof(block));
    memset(inode_blocks, 0, sizeof(block) * 5);
    memset(data_blocks, 0, sizeof(block) * 56);

    byteLine *pointer = (byteLine *) i_bmap_blocks;
    (*pointer).b7 = 1;
    (*pointer).b6 = 1;
    (*pointer).b5 = 1;

    pointer = (byteLine *) d_bmap_blocks;
    (*pointer).b7 = 1;

    inode_blocks[0].fsize = 320;
    inode_blocks[0].blocks = 1;
    inode_blocks[0].pointer[0] = 0;

    rootBlock = *(root_block *)&data_blocks[0];
}

int checkBitmap(void *bmap, const unsigned int num, unsigned int *data_pointer) {
    byteLine *mapPoint = (byteLine *) bmap;
    unsigned int remain = 0;
    int lineOffset = 8;
    if (data_pointer != NULL){
       // printf("d_bmap check\n");
    }else{
       // printf("i_bmap check\n");
    }
    for (int i = 0; i < 10; i++) {
        if (mapPoint[i].b7 == 0) {
            mapPoint[i].b7 = 1;
          //  printf("i is %d\n",i);
          //  printf("b7 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
          //      printf("new data_block offset: %d\n",lineOffset * i);
                data_pointer[remain] = lineOffset * i;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i;
            }
        }
        if (mapPoint[i].b6 == 0) {
            mapPoint[i].b6 = 1;
         //   printf("i is %d\n",i);
          //  printf("b6 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
           //     printf("new data_block offset: %d\n",lineOffset * i + 1);
                data_pointer[remain] = lineOffset * i + 1;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i + 1;
            }
        }
        if (mapPoint[i].b5 == 0) {
            mapPoint[i].b5 = 1;
          //  printf("i is %d\n",i);
          //  printf("b5 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
          //      printf("new data_block offset: %d\n",lineOffset * i + 2);
                data_pointer[remain] = lineOffset * i + 2;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i + 2;
            }
        }
        if (mapPoint[i].b4 == 0) {
            mapPoint[i].b4 = 1;
           // printf("i is %d\n",i);
          //  printf("b4 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
          //     printf("new data_block offset: %d\n",lineOffset * i + 3);
                data_pointer[remain] = lineOffset * i + 3;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i + 3;
            }
        }
        if (mapPoint[i].b3 == 0) {
            mapPoint[i].b3 = 1;
           // printf("i is %d\n",i);
           // printf("b3 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
            //    printf("new data_block offset: %d\n",lineOffset * i + 4);
                data_pointer[remain] = lineOffset * i + 4;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i + 4;
            }

        }
        if (mapPoint[i].b2 == 0) {
            mapPoint[i].b2 = 1;
           // printf("i is %d\n",i);
           // printf("b2 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
            //    printf("new data_block offset: %d\n",lineOffset * i + 5);
                data_pointer[remain] = lineOffset * i + 5;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i + 5;
            }

        }
        if (mapPoint[i].b1 == 0) {
            mapPoint[i].b1 = 1;
           // printf("i is %d\n",i);
          //  printf("b1 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
           //     printf("new data_block offset: %d\n",lineOffset * i + 6);
                data_pointer[remain] = lineOffset * i + 6;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i + 6;
            }
        }
        if (mapPoint[i].b0 == 0) {
            mapPoint[i].b0 = 1;
           // printf("i is %d\n",i);
           // printf("b0 check mapPoint[i] = %X\n",mapPoint[i].val);
            if (data_pointer != NULL) {
              //  printf("new data_block offset: %d\n",lineOffset * i + 7);
                data_pointer[remain] = lineOffset * i + 7;
            }
            remain++;
            if (remain >= num) {
                return lineOffset * i + 7;
            }
        }
    }
}

int isExists(const char *name) {
    for (int i = 0; i < rootOffset; i++) {
        if (rootBlock.entries[i].inum != 0 && rootBlock.entries[i].inum != 1) {
            if (rootBlock.entries[i].Name[0] == name[0] && rootBlock.entries[i].Name[1] == name[1]) {
               // printf("i found file name %s with inum %d\n",rootBlock.entries[i].Name,rootBlock.entries[i].inum);
                return rootBlock.entries[i].inum;
            }
        }
    }
   // printf("i couldn't found file name %s\n",name);
    return 0;
};

void fillData(unsigned int *pointer, const unsigned int blocks, const unsigned int lastBlocks, char var) {
    for (int i = 0; i < blocks; i++) {
        unsigned int index = pointer[i];
    //    printf("fillData i: %d and offset : %d\n",i,index);
        memset(data_blocks+index, var, sizeof(block));
    }
    if(lastBlocks != 0){
        unsigned int index = *(pointer + blocks);
   //     printf("fillData offset : %d\n",index);
        memset(data_blocks+index, var, lastBlocks);
    }
  //  printf("fill data with: %c within %d\n",var, sizeof(block)*blocks + lastBlocks);
}

void putInRoot(const char *name, const unsigned inum) {
    if (rootOffset < 80) {
        rootOffset++;
    }
    for (int i = 0; i < rootOffset; i++) {
        if (rootBlock.entries[i].inum == 0) {
            rootBlock.entries[i].inum  = inum;
            rootBlock.entries[i].Name[0] = name[0];
            rootBlock.entries[i].Name[1] = name[1];
            rootBlock.entries[i].Name[2] = '\0';

        //    printf("root put: %s\n",rootBlock.entries[i].Name);
            break;
        }
    }
}

void writeFile(const char *name, const unsigned int fsize) {
    if (isExists(name) != 0) {
        printf("Already exists\n");
        return;
    }
    unsigned int temp = fsize / 4096;
    unsigned int lastBlock = fsize % 4096;
    unsigned int needBlockNum = temp;
    if (lastBlock > 0) {
        needBlockNum++;
    }

    if (usedDataBlock < needBlockNum) {
        printf("No space\n");
        return;
    }
    int inum = checkBitmap(i_bmap_blocks, 1, NULL);
 //   printf("%s 's new inum is %d\n",name,inum);
    if (inum == -1) {
        printf("No space\n");
        return;
    }

    inode_blocks[inum].fsize = fsize;
    inode_blocks[inum].blocks = needBlockNum;

    checkBitmap(d_bmap_blocks, needBlockNum, inode_blocks[inum].pointer);
    for(int i = 0;i<needBlockNum;i++){
      //  printf("direct pointer ith: %d\n",inode_blocks[inum].pointer[i]);
    }
    if(lastBlock == 0){
        fillData(inode_blocks[inum].pointer, needBlockNum, 0, *name);
    }else{
        fillData(inode_blocks[inum].pointer, temp, lastBlock, *name);
    }

    putInRoot(name, inum);
}

void readFile(const char *name, const unsigned int size) {
    int inum = isExists(name);
    if (inum == 0) {
        printf("No such file\n");
        return;
    }
  //  printf("fild %s 's inode is %d\n",name,inum);
    unsigned int fsize = inode_blocks[inum].fsize;
    unsigned int min;
    if (size > fsize) {
        min = fsize;
    } else {
        min = size;
    }

    unsigned int needBlockNum = min / BLOCK_SIZE;
    unsigned int lastBlock = min % BLOCK_SIZE;

   //  printf("need to read block: %d, last: %d\n",needBlockNum,lastBlock);

    for (int i = 0; i < needBlockNum; i++) {
        unsigned int index = inode_blocks[inum].pointer[i];
      //  printf("data_block offset: %d\n",index);
        unsigned int flag = 0;
        while (flag < BLOCK_SIZE) {
            putchar(data_blocks[index].byteLines[flag++].val);
        }
    }
    unsigned int flag = 0;
    unsigned int index = inode_blocks[inum].pointer[needBlockNum];
  //  printf("data_block offset: %d\n",index);
    while (flag < lastBlock) {
        putchar(data_blocks[index].byteLines[flag++].val);
    }
    putchar('\n');
}

void zeroBitmap(void *bmap, const int index) {
    unsigned int lineIndex = index / 8;
    unsigned int lineOffset = index % 8;

    byteLine *pointer = bmap;
    switch (lineOffset) {
        case 0:
            pointer[lineIndex].b7 = 0;
            break;
        case 1:
            pointer[lineIndex].b6 = 0;
            break;
        case 2:
            pointer[lineIndex].b5 = 0;
            break;
        case 3:
            pointer[lineIndex].b4 = 0;
            break;
        case 4:
            pointer[lineIndex].b3 = 0;
            break;
        case 5:
            pointer[lineIndex].b2 = 0;
            break;
        case 6:
            pointer[lineIndex].b1 = 0;
            break;
        case 7:
            pointer[lineIndex].b0 = 0;
            break;
    }
}

void deleteFromRoot(const char *name) {
    for (int i = 0; i < rootOffset; i++) {
        if (rootBlock.entries[i].inum != 0 && rootBlock.entries[i].inum != 1) {
            if (rootBlock.entries[i].Name[0] == name[0] && rootBlock.entries[i].Name[1] == name[1]) {
              //  printf("i delete file %s with inum %d\n",rootBlock.entries[i].Name,rootBlock.entries[i].inum);
                rootBlock.entries[i].inum = 0;
                break;
            }
        }
    }
}

void deleteFile(const char *name) {
    int inum = isExists(name);
    if (inum == 0) {
        printf("No such file\n");
        return;
    }
    unsigned int blocks = inode_blocks[inum].blocks;
    for (int i = 0; i < blocks; i++) {
        unsigned int index = inode_blocks[inum].pointer[i];
        zeroBitmap(d_bmap_blocks, index);
    }
    zeroBitmap(i_bmap_blocks, inum);
    deleteFromRoot(name);
}

void showDump() {
    printf("----super block----\n");
    unsigned int num =0;
    while(num < BLOCK_SIZE){
        int num1 = super_blocks->byteLines[num].h1;
        int num2 = super_blocks->byteLines[num++].h0;
        printf("%X%X",num1,num2);
    }
    printf("\n-------------------\n");
    printf("----i_bmap block----\n");
    num =0;
    while(num < BLOCK_SIZE){
        int num1 = i_bmap_blocks->byteLines[num].h1;
        int num2 = i_bmap_blocks->byteLines[num++].h0;
        printf("%X%X",num1,num2);
    }
    printf("\n-------------------\n");
    printf("----d_bmap block----\n");
    num =0;
    while(num < BLOCK_SIZE){
        int num1 = d_bmap_blocks->byteLines[num].h1;
        int num2 = d_bmap_blocks->byteLines[num++].h0;
        printf("%X%X",num1,num2);
    }
    printf("\n-------------------\n");
    printf("----inode block----\n");
    num =0;
    while(num < BLOCK_SIZE * 5){
        int num1 = ((block*)inode_blocks)->byteLines[num].h1;
        int num2 = ((block*)inode_blocks)->byteLines[num++].h0;
        printf("%X%X",num1,num2);
    }
    inode_blocks = (inode *) malloc(sizeof(block) * 5);
    printf("\n-------------------\n");

    printf("----data block----\n");
    num =0;
    while(num < BLOCK_SIZE * 56){
        int num1 = data_blocks->byteLines[num].h1;
        int num2 = data_blocks->byteLines[num++].h0;
        printf("%X%X",num1,num2);
    }
    data_blocks = (block *) malloc(sizeof(block) * 56);
    printf("\n-------------------\n");
}


int main(int argc, char *argv[]) {
    FILE *fd = NULL;
    char line[1024];
    char fileName[3];
    char command;
    unsigned int size = 0;
/*
    if (argc != 2) {
        printf("ku_fs: Wrong number of arguments\n");
        return 1;
    }

    fd = fopen(argv[1], "r");
    if (!fd) {
        printf("ku_fs: Fail to open the input file\n");
        return 1;
    }
*/
    initPartition();

    while (fgets(line, 1024, stdin) != NULL) {
        sscanf(line, "%s %c %ud", fileName, &command, &size);
        switch (command) {
            case 'w':
                writeFile(fileName, size);
                break;
            case 'r':
                readFile(fileName, size);
                break;
            case 'd':
                deleteFile(fileName);
                break;
            default:
                goto DUMP;
                break;
        }
    }
DUMP:
    showDump();
    return 0;
}
