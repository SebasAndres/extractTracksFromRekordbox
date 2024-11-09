#include <iostream>
// external libraries
#include <filesystem>
#include <fstream>
// #include <json/json.h>
// data structures
#include <string>
#include <vector>
#include <map>
// internal
#include "defines.h"

using namespace std;
namespace fs = filesystem;

fs::path getDestinyName(fs::path originalPath){
      // dst_folder ++ filename
      return DST_FOLDER / originalPath.filename();
}

uint8_t isAllowedExtension(const string& extension){
      // valida si es uno de los formatos aceptados
      for(string ext: ALLOWED_EXTENSIONS){
            if (ext==extension)    
                  return 1;
      }
      return 0;
}

void registerNotCopiedFile(fs::path* notCopied, uint32_t& count, const fs::path& path){
      // actualiza la flag con los registros no copiados
      notCopied[count] = path;
      count++;
}

int main(void){

     // vamos a realizar una copia al momento de leer la carpeta CONTENTS
     // por cada subcarpeta obtener y copiar los archivos terminados en .mp3 o .wav 

      fs::path* notCopiedFiles = new fs::path[MAX_FILES];
      uint32_t notCopiedCounter = 0;
      uint32_t copiedCounter = 0;
      uint8_t earlyStop = 0;

      if (!(fs::exists(CONTENTS_FOLDER_PATH))) { cerr << "ERROR: No existe la carpeta de origen" << endl; return 0; } 

      cout << endl;
      cout << "Copiando archivos." << endl;
      for (const auto& artistFolder: fs::directory_iterator(CONTENTS_FOLDER_PATH)){               // contents/artists
            for (const auto& sourceDownloadFolder: fs::directory_iterator(artistFolder)){       // contents/artists/source
                  for (const auto& file: fs::directory_iterator(sourceDownloadFolder)){         // contents/artists/source/file

                        fs::path srcPath = file.path();
                        string extension = srcPath.extension();

                        // irregular file or invalid extension
                        if (!(file.is_regular_file()) | !(isAllowedExtension(extension))) { registerNotCopiedFile(notCopiedFiles, notCopiedCounter, srcPath); continue; }  

                        // copy file
                        fs::path dstPath = getDestinyName(srcPath);
                        try{
                              fs::copy(srcPath, dstPath, fs::copy_options::overwrite_existing);
                              cout << "(*) ["<< copiedCounter << "] Copiado exitosamente " << srcPath << endl;

                              copiedCounter++;
                              if (copiedCounter >= MAX_FILES) { earlyStop = 1; break; }
                        }
                        catch(const fs::filesystem_error& e){
                              cout << e.what() << endl;
                              registerNotCopiedFile(notCopiedFiles, notCopiedCounter, srcPath);
                        };
                  }
                  if (copiedCounter >= MAX_FILES) { break; } 
            }
            if (copiedCounter >= MAX_FILES) { break; }
      }

      if(notCopiedCounter > 0){
            cout << endl;
            cout << "No se copiaron " << notCopiedCounter << " archivos" << endl;
            for(uint32_t j=0; j<notCopiedCounter; j++)
                  cout << "(*) No se copió: " << notCopiedFiles[j] << endl;
      }

      if(earlyStop){
            cout << endl;
            cout << "El programa se terminó por early stop despues de " << MAX_FILES << " archivos copiados." << endl;
      }

      delete[] notCopiedFiles;
}
