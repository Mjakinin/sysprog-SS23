#include "../lib/operations.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int get_inode_index(file_system *fs, char *path)
{
  if(path == NULL)
  {
    return -1; //Error
  }

  int current_index = fs->root_node;
  char slash[1] = "/";
  char *current_path = strtok(path, "/");
  
  while(current_path != NULL)
  {
    inode *current_inode = &(fs->inodes[current_index]);
    int hit = 0;
    
    for(int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
    {
      if(strcmp(fs->inodes[current_inode->direct_blocks[i]].name, current_path) == 0)
      {
        current_index = current_inode->direct_blocks[i];
        hit = 1;
        break;
      }
    }
    
    if(!hit)
    {
      return -1;
    }
    
    current_path = strtok(NULL, "/");
    
    if(current_path != NULL)
    {
      strcat(slash, current_path);
      strcat(slash, "/");
    }

  }


  return current_index;
}









void delete_inodes(file_system *fs, int inode_index) //für rm
{
  inode *deleted_node = &(fs->inodes[inode_index]);
    
  if(deleted_node->n_type == directory)
  {
    for(int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
    {
      int used_block = deleted_node->direct_blocks[i];
        
      if(used_block != -1)
      {
        delete_inodes(fs, used_block);
      }
    }
  }

  deleted_node->n_type = free_block;
  fs->free_list[inode_index] = 1;

  for(int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
  {
    int used_block = deleted_node->direct_blocks[i];
        
    if(used_block != -1)
    {
      fs->free_list[used_block] = 1;
      deleted_node->direct_blocks[i] = -1;
    }
  }
}











int free_block_index(file_system *fs) //für import
{
  for(int i = 0; i  <  fs->s_block->num_blocks; i++)
  {
    if(fs->free_list[i] == 1)
    {
      return i;
    }
  }
  return -1;
}





















int 
fs_mkdir(file_system *fs, char *path) //RICHTIGER ANSATZ
{
  if(fs == NULL || path == NULL)
  {
    return -1; //Error
  }

  if(path[0] != '/' || strlen(path) <= 1)
  {
    return -1; //Error
  }



  char *parent_path = strdup(path);

  fprintf(stderr,"%s \n",parent_path);

  //Aufteilen des Pfads
  char *current_path = strtok(path, "/");
  char *new_dir = NULL;
  while(current_path != NULL)
  {
    new_dir = current_path; 
    current_path = strtok(NULL, "/");
  }



 fprintf(stderr,"%s \n",new_dir);


  //parent_path erstellen
  if(new_dir != NULL)
  {
    size_t new_file_length = strlen(new_dir);
  
    parent_path[strlen(parent_path) - new_file_length -1] = '\0';
  }



  fprintf(stderr,"%s \n",parent_path);



  //freien Inode finden
  int new_inode_index = find_free_inode(fs);
  if(new_inode_index == -1)
  {
    fprintf(stderr,"raus, kein freier inode\n");
    return -1; //Error
  }
  inode *new_inode = &(fs->inodes[new_inode_index]);









  //parent_index finden
  int parent_index = get_inode_index(fs,parent_path);
  if(parent_index == -1)
  {
    return -1; //Error
  }

  
  

  
  
  //initialisieren
  new_inode->n_type = directory;
  strncpy(new_inode->name, new_dir, NAME_MAX_LENGTH);  
  new_inode->parent = parent_index;





  //vom parent freien directblock finden
  int freier_index = -1;
  for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
  {
    if(fs->inodes[parent_index].direct_blocks[i] == -1)
    {
      freier_index = i;
      break;
    }
  }
  if(freier_index == -1)
  {
    return -1; //Error
  }
  //übernehmen der Inodenummer in den parent direct block
  fs->inodes[parent_index].direct_blocks[freier_index] = new_inode_index;





  return 0;
}

int
fs_mkfile(file_system *fs, char *path_and_name) //RICHTIGER ANSATZ
{
  if(fs == NULL || path_and_name == NULL)
  {
    return -1; //Error
  }

  if(path_and_name[0] != '/' || strlen(path_and_name) <= 1)
  {
    return -1; //Error
  }



  char *parent_path = strdup(path_and_name);



  //Aufteilen des Pfads
  char *current_path = strtok(path_and_name, "/");
  char *new_file = NULL;
  while(current_path != NULL)
  {
    new_file = current_path;
    current_path = strtok(NULL, "/");
  }





  //parent_path erstellen
  if(new_file != NULL)
  {
    size_t new_file_length = strlen(new_file);
    parent_path[strlen(parent_path) - new_file_length -1] = '\0';
  }





  //parent_index finden
  int parent_index = get_inode_index(fs,parent_path);
  if(parent_index == -1)
  {
    return -1; //Error
  }





  //überprüfen ob parent = directory
  if(fs->inodes[parent_index].n_type != directory)
  {
    return -1; //Error
  }







  //überprüfen ob die Datei bereits existiert
  for(int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
  {
    int block_index = fs->inodes[parent_index].direct_blocks[i];

    if(fs->inodes[block_index].n_type == reg_file && strcmp(fs->inodes[block_index].name, new_file) == 0)
    {
      return -2; //Existiert schon
    }
  }







  //freien Inode finden
  int new_inode_index = find_free_inode(fs);
  if(new_inode_index == -1)
  {
    return -1; //Error
  }
  inode *new_inode = &(fs->inodes[new_inode_index]);






  //initialisieren
  new_inode->n_type = reg_file;
  strncpy(new_inode->name, new_file, NAME_MAX_LENGTH);
  new_inode->parent = parent_index;







  //vom parent freien directblock finden
  int freier_index = -1;
  for(int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
  {
    if(fs->inodes[parent_index].direct_blocks[i] == -1)
    {
      freier_index = i;
      break;
    }
  }
  if(freier_index == -1)
  {
    return -1; //Error
  }
  //übernehmen der Inodenummer in den parent direct block
  fs->inodes[parent_index].direct_blocks[freier_index] = new_inode_index;





  return 0; // Erfolgreich erstellt
}

char *
fs_list(file_system *fs, char *path) //FUNKTIONIERT
{
  //index finden
  int path_index = get_inode_index(fs,path);


  char* result = malloc(BLOCK_SIZE);
  result[0] = '\0';

  for(int i = 0; i < fs->s_block->num_blocks; i++)
  {
    inode* node = &(fs->inodes[i]);

    if(node->parent == path_index)
    {
      if(node->n_type == directory)
      {
        strcat(result, "DIR ");
      }
      else if(node->n_type == reg_file)
      {
        strcat(result, "FIL ");
      }
        
      strcat(result, node->name);
      strcat(result, "\n");
    }
  }

  
  return result;
}

int
fs_writef(file_system *fs, char *filename, char *text) //RICHTIGER ANSATZ
{
  //index finden
  int inode_index = get_inode_index(fs, filename);
  if(inode_index == -1)
  {
    return -1; //Error
  }



  inode* file_inode = &(fs->inodes[inode_index]);

  if(file_inode->n_type != reg_file)
  {
    return -1; //Error
  }



  

  //finde freien datenblock
  int free_block = -1;
  for(int i = 0; i < fs->s_block->num_blocks; i++)
  {
    if(fs->free_list[i])
    {
      free_block = i;
      fs->free_list[i] = 0;
      break;
    }
  }
  if(free_block == -1)
  {
    return -2; //Error
  }





  //schreibe daten in datenblock
  size_t text_len = strlen(text);
  int needed_blocks = (text_len + BLOCK_SIZE - 1) / BLOCK_SIZE;
  int written_blocks = 0;
  int current_block = free_block;

  while(written_blocks < needed_blocks && current_block < DIRECT_BLOCKS_COUNT)
  {
    data_block* block = &(fs->data_blocks[current_block]);

    size_t remaining_text_len = text_len - (written_blocks * BLOCK_SIZE); //menge des textes die in diesem block geschrieben werden soll
    size_t block_text_len = (remaining_text_len < BLOCK_SIZE) ? remaining_text_len : BLOCK_SIZE;

    memcpy(block->block, text + (written_blocks * BLOCK_SIZE), block_text_len); //schreib den text in den block
    block->size = block_text_len;
    file_inode->size += block_text_len; //inode größe aktualisieren
    file_inode->direct_blocks[written_blocks] = current_block; //directblock aktualisieren

    written_blocks++;
    current_block++;
  }

  fs->s_block->free_blocks -= written_blocks; //aktualisiere superblöcke




  return text_len;
}

uint8_t *
fs_readf(file_system *fs, char *filename, int *file_size) //RICHTIGER ANSATZ
{
  //index finden
	int inode_index = get_inode_index(fs, filename);
  if(inode_index == -1)
  {
    return NULL; //Error
  }



  inode *file_inode = &(fs->inodes[inode_index]);

  if(file_inode->n_type != reg_file)
  {
    return NULL; //Error
  }




  int num_blocks = (file_inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
  int buffer_size = num_blocks * BLOCK_SIZE;

  uint8_t *buffer = (uint8_t *)malloc(buffer_size);
  if(buffer == NULL)
  {
    return NULL; //Error
  }

  int buffer_index = 0;

  for(int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
  {
    int used_block = file_inode->direct_blocks[i];

    if(used_block == -1)
    {
      break; //kein datenblock vorhanden
    }
    data_block *block = &(fs->data_blocks[used_block]);
    memcpy(buffer + buffer_index, block->block, block->size);
    buffer_index += block->size;
  }
  *file_size = file_inode->size;


  return buffer;
}

int
fs_rm(file_system *fs, char *path) //RICHTIGER ANSATZ
{
  //index finden
  int inode_index = get_inode_index(fs, path);
  fprintf(stderr, "Zu Entfernendes Path: %s\n", path);
  if(inode_index == -1)
  {
    return -1; //Error
  }



    
  inode *node = &(fs->inodes[inode_index]);
    
  if(node->n_type == reg_file)
  {
    delete_inodes(fs, inode_index); //rekursiv löschen
  }
  else if(node->n_type == directory)
  {
    if(strcmp(path, "/") == 0)
    {
      return -1; //Error
    }
        
    delete_inodes(fs, inode_index); //rekursiv löschen
        
    //direct blocks freen
    int parent_index = node->parent;
    inode *parent_node = &(fs->inodes[parent_index]);
        
    for(int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
    {
      if(parent_node->direct_blocks[i] == inode_index)
      {
        parent_node->direct_blocks[i] = -1;
        break;
      }
    }
  }
  else
  {
    return -1;
  }

    
  inode_init(node); //inode als leer initialisieren
  fs->s_block->free_blocks += 1; //superblock aktualisieren


    
  return 0;
}

int
fs_import(file_system *fs, char *int_path, char *ext_path) //FUNKTIONIERT
{
  FILE *ext_file = fopen(ext_path, "rb");
  if(ext_file == NULL)
  {
    return -1; //Error
  }



  //index finden
  int intern_index = get_inode_index(fs, int_path); //interner pfad index suchen
  if(intern_index == -1)
  {
    fclose(ext_file);
    return -1; //Error
  }



  

  inode *int_inode = &(fs->inodes[intern_index]);
  
  fseek(ext_file, 0, SEEK_END);
  size_t ext_size = ftell(ext_file);
  fseek(ext_file, 0, SEEK_SET);
  




  size_t num_blocks = (ext_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

  if(num_blocks > DIRECT_BLOCKS_COUNT)
  {
    fclose(ext_file);
    return -1; //Error
  }
  




  //daten von extern zu intern kopieren
  for(int i = 0; i < num_blocks; i++)
  {
    int free_block = int_inode->direct_blocks[i];
    
    if(free_block == -1)
    {
      free_block = free_block_index(fs);

      if(free_block == -1)
      {
        fclose(ext_file);
        return -1; //Error
      }
      
      int_inode->direct_blocks[i] = free_block;
      fs->free_list[free_block] = 0;
    }
    
    //daten aus extern lesen und in datenblock kopieren
    size_t copy_size = (i == num_blocks - 1) ? (ext_size % BLOCK_SIZE) : BLOCK_SIZE;
    fread(fs->data_blocks[free_block].block, 1, copy_size, ext_file);
    fs->data_blocks[free_block].size = copy_size;
  }


  int_inode->size = ext_size;
  fclose(ext_file);



  return 0;
}

int
fs_export(file_system *fs, char *int_path, char *ext_path) //FUNKTIONIERT
{
  //index finden
  int intern_inode_index = get_inode_index(fs, int_path);
  if(intern_inode_index == -1)
  {
    return -1; //Error
  }






  inode *file_inode = &(fs->inodes[intern_inode_index]);

  if(file_inode->n_type != reg_file)
  {
    return -1; //Error
  }

  FILE *external_file = fopen(ext_path, "wb");
  if(external_file == NULL)
  {
    return -1; //Error
  }




  int num_blocks = (file_inode->size + BLOCK_SIZE - 1) / BLOCK_SIZE;

  for(int i = 0; i < num_blocks; i++)
  {
    int free_block = file_inode->direct_blocks[i];

    if(free_block == -1)
    {
      break; //kein datenblock vorhanden
    }

    data_block *block = &(fs->data_blocks[free_block]);

    fwrite(block->block, sizeof(uint8_t), block->size, external_file);
  }
  fclose(external_file);




  return 0;
}