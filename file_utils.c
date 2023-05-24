#include "file_utils.h"
#include "Client.h"
#include "Produit.h"
#include <stdio.h>
#include <string.h>

#define LOW_STOCK_THRESHOLD 5

void writeClientToFile(Client client) {
    FILE *file = fopen("clients.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s %s [", client.nom, client.prenom);
        for (int i = 0; i < client.nbAchats; i++) {
            fprintf(file, "%d %s, ", client.historiqueAchats[i].quantite, client.historiqueAchats[i].nomProduit);
        }
        fprintf(file, "]\n");
        fclose(file);
    }
}
Client readClientFromFile(char *nomClient) {
    Client client;
    FILE *file = fopen("clients.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%s %s [", client.nom, client.prenom) != EOF) {  
          client.nbAchats = 0;
            while (fscanf(file, "%d %[^,], ", &client.historiqueAchats[client.nbAchats].quantite, client.historiqueAchats[client.nbAchats].nomProduit) == 2) {
                client.nbAchats++;
            }
            // lire le dernier ']' à la fin de la ligne
            fscanf(file, "]");
            if (strcmp(client.nom, nomClient) == 0) {
                fclose(file);
                return client;
            }
        }
        fclose(file);
    }
    // Initialise le client à vide si le client n'a pas été trouvé
    memset(&client, 0, sizeof(Client));
    return client;
}

void writeProductToFile(Produit produit) {
  FILE *file = fopen("produits.txt", "a");
  if (file != NULL) {
    fprintf(file, "%s %d %d %.2f %d %d %s\n", produit.nom, produit.reference,
            produit.quantite, produit.prix, produit.taille, produit.categorie,
            produit.datePeremption);
    fclose(file);
  }
}

Produit readProductFromFile(char *nomProduit) {
  Produit produit = {"", 0, 0, 0.0, 0, 0, ""};
  FILE *file = fopen("produits.txt", "r");
  if (file != NULL) {
    while (fscanf(file, "%s %d %d %f %d %d %s\n", produit.nom,
                  &produit.reference, &produit.quantite, &produit.prix,
                  &produit.taille, &produit.categorie,
                  produit.datePeremption) != EOF) {
      if (strcmp(produit.nom, nomProduit) == 0) {
        fclose(file);
        return produit;
      }
    }
    fclose(file);
  }
  return produit; // Retourne un produit vide si le produit n'a pas été trouvé
}

void deleteClientFromFile(char *nomClient) {
  Client client;
  FILE *originalFile = fopen("clients.txt", "r");
  FILE *tempFile = fopen("temp.txt", "w");

  if (originalFile != NULL && tempFile != NULL) {
    while (fscanf(originalFile, "%s %s [", client.nom, client.prenom) != EOF) {
      client.nbAchats = 0;
      while (fscanf(originalFile, "%d %[^,], ", &client.historiqueAchats[client.nbAchats].quantite, client.historiqueAchats[client.nbAchats].nomProduit) == 2) {
                client.nbAchats++;
      }
      // lire le dernier ']' à la fin de la ligne
      fscanf(originalFile, "]");
      // Écrire seulement les clients dont le nom ne correspond pas à nomClient
      // dans le fichier temporaire
      if (strcmp(client.nom, nomClient) != 0) {
        fprintf(tempFile, "%s %s [", client.nom, client.prenom);
        for (int i = 0; i < client.nbAchats; i++) {
            fprintf(tempFile, "%d %s, ", client.historiqueAchats[i].quantite, client.historiqueAchats[i].nomProduit);
        }
        fprintf(tempFile, "]\n");
      }
    }
    fclose(originalFile);
    fclose(tempFile);

    // Supprimer l'ancien fichier et renommer le nouveau
    remove("clients.txt");
    rename("temp.txt", "clients.txt");
  }
}

void updateProductInFile(Produit produit) {
  Produit produitTemp;
  FILE *originalFile = fopen("produits.txt", "r");
  FILE *tempFile = fopen("temp.txt", "w");

  if (originalFile != NULL && tempFile != NULL) {
    while (fscanf(originalFile, "%s %d %d %f %d %d %s\n", produitTemp.nom,
                  &produitTemp.reference, &produitTemp.quantite,
                  &produitTemp.prix, &produitTemp.taille,
                  &produitTemp.categorie, produitTemp.datePeremption) != EOF) {
      // Si le nom du produit correspond à celui que nous voulons mettre à jour,
      // on écris les nouvelles informations du produit dans le fichier temporaire
      if (strcmp(produitTemp.nom, produit.nom) == 0) {
        fprintf(tempFile, "%s %d %d %.2f %d %d %s\n", produit.nom,
                produit.reference, produit.quantite, produit.prix,
                produit.taille, produit.categorie, produit.datePeremption);
      } else {
               // fichier temporaire
        fprintf(tempFile, "%s %d %d %.2f %d %d %s\n", produitTemp.nom,
                produitTemp.reference, produitTemp.quantite, produitTemp.prix,
                produitTemp.taille, produitTemp.categorie,
                produitTemp.datePeremption);
      }
    }
    fclose(originalFile);
    fclose(tempFile);

    // Supprime l'ancien fichier et renommer le nouveau
    remove("produits.txt");
    rename("temp.txt", "produits.txt");
  }
}

void updateClientInFile(Client client) {
    Client clientTemp;
    char historiqueBuffer[256];
    FILE *originalFile = fopen("clients.txt", "r");
    FILE *tempFile = fopen("temp.txt", "w");
    unsigned char client_found = 0;

    if (originalFile != NULL && tempFile != NULL) {
        while (fscanf(originalFile, "%s %s %[^\n]", clientTemp.nom, clientTemp.prenom, historiqueBuffer) != EOF) {
            if (strcmp(clientTemp.nom, client.nom) == 0 && strcmp(clientTemp.prenom, client.prenom) == 0) {
                fprintf(tempFile, "%s %s [", client.nom, client.prenom); 
                formaterHistorique(&client);
                fprintf(tempFile, "%s]\n", client.historiqueFormate);
                client_found = 1;
            } else {
                fprintf(tempFile, "%s %s %s\n", clientTemp.nom, clientTemp.prenom, historiqueBuffer);
            }
        }
        /*if (!client_found)  {
                fprintf(tempFile, "%s %s", clientTemp.nom, clientTemp.prenom);
                for (int i = 0; i < clientTemp.nbAchats; i++) {
                    fprintf(tempFile, ", %d %s", clientTemp.historiqueAchats[i].quantite, clientTemp.historiqueAchats[i].nomProduit);
                }
                fprintf(tempFile, "\n");
            }*/

        if (!client_found) {
          printf("Error, client not found!\n");
        }
      
        fclose(originalFile);
        fclose(tempFile);

        remove("clients.txt");
        rename("temp.txt", "clients.txt");
    }
}


Produit *getLowStockProducts(int *numberOfProducts) {
    Produit *lowStockProducts = NULL;
    Produit produitTemp;
    *numberOfProducts = 0;
    FILE *file = fopen("produits.txt", "r");

    if (file != NULL) {
        // Parcourir le fichier pour compter combien de produits ont un stock bas
        while (fscanf(file, "%s %d %d %f %d %d %s\n", produitTemp.nom, &produitTemp.reference, 
                      &produitTemp.quantite, &produitTemp.prix, &produitTemp.taille, 
                      &produitTemp.categorie, produitTemp.datePeremption) != EOF) {
            if (produitTemp.quantite <= LOW_STOCK_THRESHOLD) {
                (*numberOfProducts)++;
            }
        }

        // Allouer de la mémoire pour le tableau de produits
        lowStockProducts = malloc((*numberOfProducts) * sizeof(Produit));
        if (lowStockProducts == NULL) {
            // gestion d'erreur d'allocation mémoire
            fclose(file);
            return NULL;
        }

        rewind(file);  // Remettre le curseur du fichier au début
        int i = 0;

        // Parcourir le fichier une deuxième fois pour remplir le tableau avec les produits en faible stock
        while (fscanf(file, "%s %d %d %f %d %d %s\n", produitTemp.nom, &produitTemp.reference, 
                      &produitTemp.quantite, &produitTemp.prix, &produitTemp.taille, 
                      &produitTemp.categorie, produitTemp.datePeremption) != EOF) {
            if (produitTemp.quantite <= LOW_STOCK_THRESHOLD) {
                lowStockProducts[i] = produitTemp;
                i++;
            }
        }

        fclose(file);
    }
    return lowStockProducts;
}

void checkAndCreateFile(const char *filename) {
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        fclose(file);
    } else {
        printf("Impossible de créer ou d'ouvrir le fichier: %s\n", filename);
    }
}

void checkAndCreateClientAndProductFiles() {
    checkAndCreateFile("clients.txt");
    checkAndCreateFile("produits.txt");
}

float getProductPrice(char* nomProduit) {
    Produit produit = readProductFromFile(nomProduit);
    return produit.prix;
}

Produit readProductByReferenceFromFile(int reference) {
    FILE *file = fopen("produits.txt", "r");
    Produit produit;
    while (fscanf(file, "%s %d %d %f %d %d %s", produit.nom, &produit.reference, &produit.quantite, &produit.prix, &produit.taille, &produit.categorie, produit.datePeremption) != EOF) {
        if (produit.reference == reference) {
            fclose(file);
            return produit;
        }
    }
    fclose(file);
    produit.reference = -1; // Retourne un produit avec une référence invalide si le produit n'est pas trouvé
    return produit;
}

Produit readProductByCategoryFromFile(int categorie) {
    FILE *file = fopen("produits.txt", "r");
    Produit produit;
    while (fscanf(file, "%s %d %d %f %d %d %s", produit.nom, &produit.reference, &produit.quantite, &produit.prix, &produit.taille, &produit.categorie, produit.datePeremption) != EOF) {
        if (produit.categorie == categorie) {
            fclose(file);
            return produit;
        }
    }
    fclose(file);
    produit.categorie = -1; // Retourne un produit avec une catégorie invalide si le produit n'est pas trouvé
    return produit;
}

void formaterHistorique(Client *client) {
    char buffer[1000];
    buffer[0] = '\0';  // Initialise le buffer à une chaîne vide

    for (int i = 0; i < client->nbAchats; i++) {
        char achat[100];  // Buffer pour un seul achat
        sprintf(achat, " %d %s,", client->historiqueAchats[i].quantite, client->historiqueAchats[i].nomProduit);

        // Ajoute l'achat au buffer principal
        strcat(buffer, achat);
    }

    // Copie le buffer dans la structure Client
    strcpy(client->historiqueFormate, buffer);
}

void ajouterAchat(Client *client, Produit produit, int quantite) {
    Achat newAchat;
    strcpy(newAchat.nomProduit, produit.nom);
    newAchat.quantite = quantite;

    client->historiqueAchats[client->nbAchats] = newAchat;
    client->nbAchats++;

    // Formate l'historique avant de mettre à jour le client dans le fichier.
    formaterHistorique(client);

    updateClientInFile(*client);
}

