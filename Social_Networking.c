#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_USERS 50

typedef struct {
    int userID;
    char name[100];
    char password[20];
    float frndscore;
} User;

typedef struct {
    int numUsers;
    User users[MAX_USERS];
    bool adjacencyMatrix[MAX_USERS][MAX_USERS];
    int followersOfUser[MAX_USERS];
} SocialNetwork;

// function declaration's 
// before MENU data loadin...
void initSocialNetwork(SocialNetwork *network) ; //initizes the matrix with falsee [needed]
void readuserpass(SocialNetwork *network); //reads the user_pass.txt and stores it in network [needed]
void readData(SocialNetwork *network, const char *filename) ; // reads the data.txt and stores it in the network [needed]
void calculateNoOfFollowrs(SocialNetwork *network ); // find's the no of friends [needed]
// isdh

// menu
void menu(SocialNetwork *network); 
void checkadmin(SocialNetwork *network);// op1 : check's if the user is admin or not [needed]
int addNewUser(SocialNetwork *network); //op2 : add's new user[needed]
void addUserToFile(const char *filename, const char *name, const char *friends); // helper : used to add new user details and passsword details into respective files [needed]
void checkuserIsExisting(SocialNetwork *network); //op3 : check's if the user is existing or not [needed]

// functions available for USER'S
void functionsofuser(SocialNetwork *network,char *userlogedin);
int addFriends(SocialNetwork *network , char *name); //op1: add new freinds [needed]
void addOrRemoveFriendship(SocialNetwork *network, int userID1, int userID2,bool add) ; // helper : add's / removes Edges between 2 users [needed]
void addOrRemoveFriendsFromFile(char *filename ,char *userName,  char *friendslist , int add) ; // helper: update the data.txt by adding / removing the friends [needed]

void removeFriends(SocialNetwork *network,char *name); //op2 : rmeove old friends [needed]
void getCurrFriendsOfUser(SocialNetwork *network, char *name, char *finalFriends) ; // helper: gets the current friends of USER (after adding / deletin) [needed]

int followingg(SocialNetwork *network , char *name , char *show); // op3: display all the friends of particluar user. [needed]
int followers(SocialNetwork *network , char *name , char *show);
void displayUsersWithIDs(SocialNetwork *network) ; // helper : used to display all the user's with thier ID's [needed]
int areFriends(SocialNetwork *network,char *namez); // op4: used to checek if 2 users's are friends or not [needed]

void removeUserFromFile(SocialNetwork *network,char *namez) ; //op6 : used to DELETE an USER with all his friends from the "data.txt" file and his username and password is also removed from "user_pass.txt" file

void generateDotFile(SocialNetwork *network, const char *filename) ; //op7: used to create a graph.DOT file for graphviz to create a graph [needed]
char* generateColor(); // helper : to generate a random colour [needed]
void showthegraph(); // helper : Used to show the graph in chrome [needed]

void friendscoreOfUser(SocialNetwork *network , char *name); //op8:used to find the FriendScore of loggedin user [needed]

// functions available for ADMINN 
void admin(SocialNetwork *network); 
int findid(SocialNetwork *network,char nameiz[]);// helper : used to find the USER ID for a given name [needed]
void replaceFile(const char *oldFileName, const char *newFileName) ; //helper : used to handle renaming the new file and deleting the old file [needed]
void displaySocialNetwork(SocialNetwork *network) ; //op2: display's the adjMATRIX [needed]
void opendatafile(); //op7: opens the "data.txt" automatically [needed]
void openpassfile(); //op8: opens the "user_pass.txt" automatically [needed]
void friendscoreOfallUsers(SocialNetwork *network); //op9: used to find the FriendScore for all the user's [needed]

// main stuff

bool BFS(SocialNetwork *network,int src, int dest, int dist[MAX_USERS]);
void findMutualz(SocialNetwork *network , int s);
void searchbar(SocialNetwork *network,char *loggedin,char *addOrRem) ;

// function definitions.....
// before main module functions
void initSocialNetwork(SocialNetwork *network) 
{
    network->numUsers = 0;

    for (int i = 0; i < MAX_USERS; i++) 
    {
        for (int j = 0; j < MAX_USERS; j++)
        {
            network->adjacencyMatrix[i][j] = false; //set all values to false
        }
        network->followersOfUser[i] = 0;   // set all ranks,ie no of freinds to 0
    }
}

void readuserpass(SocialNetwork *network)
{
    FILE *file = fopen("user_pass.txt", "r");

    char line[100];
    int indexx = 0;
    while (fgets(line, sizeof(line), file)) 
    {
        char *username = strtok(line, ",");
        char *password = strtok(NULL, ",");

        if (username != NULL && password != NULL) 
        {
            // Remove the newline character from the password, if it exists
            int password_len = strlen(password);
            if (password_len > 0 && password[password_len - 1] == '\n') 
                password[password_len - 1] = '\0';
            
            indexx = findid(network,username);
            strcpy(network->users[indexx].password, password);
        }
    }

    fclose(file);
}

void readData(SocialNetwork *network, const char *filename) 
{
    FILE *file = fopen(filename, "r");

    char line[256];

    // First pass: Collect all users
    while (fgets(line, sizeof(line), file) != NULL) // get each line here
    {
        int line_length = strlen(line);
        if (line[line_length - 1] == '\n') 
            line[line_length - 1] = '\0'; // Remove the trailing newline

        char *name = strtok(line, ","); // Get the user's name
        int userID = network->numUsers; // Assign a unique user ID (assuming sequential)
        // addd it to the network
        network->users[userID].userID = userID;
        strcpy(network->users[userID].name , name);
        network->numUsers++;
    }
   
    // Return to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Second pass: Establish friendships
    while (fgets(line, sizeof(line), file) != NULL) // gets each line
    {
        int line_length = strlen(line);
        if (line[line_length - 1] == '\n') 
            line[line_length - 1] = '\0';
        
        char *name = strtok(line, ","); // Get the user's name, this is the user so leave it

        // Tokenize and process the friends list
        char *friend = strtok(NULL, ","); //1st friend
        while (friend != NULL) 
        {
            int friend_length = strlen(friend);
            if (friend[friend_length - 1] == '\n') 
                friend[friend_length - 1] = '\0'; // Remove the trailing newline from friend's name

            int userID = findid(network,name);
            int friendID = findid(network,friend);

            if (userID != -1 && friendID != -1)
                addOrRemoveFriendship(network, userID, friendID,true);
            else 
            {
                if(userID == -1)
                    printf("\nUser %s not found..\n" , name);
                if(friendID == -1)
                    printf("\nUser %s not found..\n" , friend);       
            }
            friend = strtok(NULL, ",");
        }
    }
    fclose(file);
}

void calculateNoOfFollowrs(SocialNetwork *network )
{
    int rank=0;
    for (int i = 0; i < network->numUsers; i++) 
    {
        rank = followers(network,network->users[i].name,NULL);
        network->followersOfUser[i] = rank ; 
    }
}

// main module functions :
void checkadmin(SocialNetwork *network)
{
    char password[10];
    printf("Enter the Admin Password : ");
    scanf("%s",password);

    if (strcmp(password,"admin")== 0)
    {
        system("cls");
        admin(network);
    }
    else
    {
        printf("\n\tWrong Password..\n\tReturning to Main Menu..\n\nEnter any key");
        getchar();
        getchar();
        system("cls");
        menu(network);
    }
}

int addNewUser(SocialNetwork *network)
{
    system("cls");
    char name[100];
    char friends[256];
    char finalfreinds[256];
    char password[20];

    printf("Enter the new user's name: ");
    scanf("%s", name);

    int existingUserID = findid(network , name);

    if (existingUserID==-1) 
    {
        int userID = network->numUsers;
        printf("Enter the new user's password: ");
        scanf("%s", password);
        int password_len = strlen(password);
        if (password[password_len - 1] == '\n')
            password[password_len - 1] = '\0';
        strcpy(network->users[userID].password,password);

        // addd it to the network
        network->users[userID].userID = userID;
        strcpy(network->users[userID].name , name);
        network->numUsers++;

        addUserToFile("user_pass.txt",name,password);
        addUserToFile("data.txt", name,"");
        printf("Add new friends to your network : \n");
        searchbar(network,name,"add");
        //addFriends(network,name);
        printf("\nUser '%s' and their friends added to the network and data.txt.\n", name);
        
        return userID;
    }
    else
    {
        printf("The User already exist's.\n");
        return 0;
    }
}

void addUserToFile(const char *filename, const char *name, const char *friends)
{
    FILE *file = fopen(filename, "a");
    fprintf(file, "%s,%s\n", name, friends);
    fclose(file);
}

void checkuserIsExisting(SocialNetwork *network)
{
    char password[10];
    char username[50];
    bool iscorrectpass = false;
    printf("\t\t\t\tLOGINN!!!\n\n");
    printf("\nEnter the Username : ");
    scanf("%s",username);
    int index = findid(network,username);

    if(index==-1)
    {
        printf("\n\tUser does not exist\n\tReturning to Main Menu..\n\nEnter any key");
        char ch;
        getchar();
        getchar();
        menu(network);
    }
    else
    {
        printf("\nEnter the User Password : ");
        scanf("%s",password);
        int password_len = strlen(password);
        if (password[password_len - 1] == '\n')
            password[password_len - 1] = '\0';

        if(strcmp(password,network->users[index].password)==0)
            iscorrectpass=true;
    }
    if(iscorrectpass)
    {
        system("cls");
        functionsofuser(network,username);
    }
    else
    {
        printf("\n\tWrong Password..\n\tReturning to Main Menu..\n\nEnter any key");
        char ch;
        getchar();
        getchar();
        system("cls");
        menu(network);
    }
}

// admin zfunctions...

int findid(SocialNetwork *network,char nameiz[]) 
{
    for (int i = 0; i < network->numUsers; i++) 
    {
        if (strcmp(network->users[i].name, nameiz) == 0) 
            return i;
    }
    return -1;
}

void displaySocialNetwork(SocialNetwork *network) 
{
    system("cls");
    printf("Social Network Graph (Adjacency Matrix):\n\t");

    for (int i = 0; i < network->numUsers; i++) 
        printf("%s\t",network->users[i].name);

    printf("\n\n");

    // Print the adjacency matrix
    for (int i = 0; i < network->numUsers; i++) 
    {
        printf("%s\t",network->users[i].name);// to display the 1st col ie names

        for (int j = 0; j < network->numUsers; j++) 
        {
            if (network->adjacencyMatrix[i][j]) 
                printf("YES\t"); // "Yes" represents a friendship
            else 
                printf("Noo\t"); // "No" represents no friendship
        }
        printf("\n");
    }
}

void friendscoreOfallUsers(SocialNetwork *network)
{
    float fsc = 0.0;
    int i,j,n;
    n = network->numUsers;
    float scorearr[n];
    int noOfFollowerz[n];
    char namez[n][50];
    
    for(int i=0;i<n;i++)
    {
        scorearr[i] = ((float)network->followersOfUser[i] / n)*100.0 ;
        noOfFollowerz[i] = network->followersOfUser[i];
        strcpy(namez[i], network->users[i].name);
    }
    
    // Sort the scorearr in descending order (bubble sort)
    for (i = 0; i < n - 1; i++) 
    {
        for (j = 0; j < n - i - 1; j++) 
        {
            if (scorearr[j] < scorearr[j + 1]) 
            {
                // Swap the elements if they're out of order
                float temps = scorearr[j];
                scorearr[j] = scorearr[j + 1];
                scorearr[j + 1] = temps;

                int tempf = noOfFollowerz[j];
                noOfFollowerz[j] = noOfFollowerz[j + 1];
                noOfFollowerz[j + 1] = tempf;

                char tempn[50] ;
                strcpy(tempn, namez[j]);
                strcpy(namez[j],namez[j + 1]);
                strcpy(namez[j + 1], tempn);                

            }
        }
    }

    // Display the sorted scores in descending order
    printf("Total No of Users  = %d \n",network->numUsers);
    printf("Infulence Scores are:\n");
    for (int i = 0; i < n; i++)
        printf("%d\tName: %s\tFollower's: %d\tScore: %.2f\n",i+1 ,namez[i] ,noOfFollowerz[i],scorearr[i]);
    
}

void openpassfile() 
{
    system("cls");
    const char *fileName = "user_pass.txt";
    HINSTANCE result = ShellExecute(NULL, "open", fileName, NULL, NULL, SW_SHOWNORMAL );
    // shell execute command
    if ((int)result > 32)
        printf("\n\n\t\tOpened \"%s\" successfully.\n", fileName);
    else
        printf("Opening \"%s\" failed.\n", fileName);
}

void opendatafile() 
{
    system("cls");
    const char *fileName = "data.txt";
    // handles the instance to a module , OS uses this value to handle exe when its loaded into mememory
    // parent window , operation , filename ,
    // Parameters passed to the application , 
    // The working directory for the application ,
    HINSTANCE result = ShellExecute(NULL, "open", fileName, NULL, NULL, SW_SHOWNORMAL );
    // shell execute command
    // ShellExecute function returns a value greater than 32 if it succeeds. 
    if ((int)result > 32)
        printf("\n\n\t\tOpened \"%s\" successfully.\n", fileName);
    else
        printf("Opening \"%s\" failed.\n", fileName);
}

// user ufunctions...

void friendscoreOfUser(SocialNetwork *network , char *name)
{
    float fsc = 0.0;
    int id = findid(network,name);
    fsc = ((float)network->followersOfUser[id] / network->numUsers)*100.0 ;
    network->users[id].frndscore = fsc;
    printf("Friend Score of %s isz %.2f %%\n",name,fsc);
}

void displayUsersWithIDs(SocialNetwork *network) 
{
    printf("User IDs:\n\n");
    int columns = 3;
    for (int i = 0; i < network->numUsers; i++) 
    {
        if (i % columns == 0 && i != 0) 
            printf("\n"); // Start a new row
        printf("ID: %d - Name: %s\t", network->users[i].userID, network->users[i].name);
    }
    printf("\n");
}

void addOrRemoveFriendship(SocialNetwork *network, int userID1, int userID2 , bool add) 
{
    if (userID1 < network->numUsers && userID2 < network->numUsers) 
    {
        if(add==true)
        {
            network->adjacencyMatrix[userID1][userID2] = true;
            printf("\nAdded %s as your friend.\n");
        }
        if(add==false)
        {
            if(network->adjacencyMatrix[userID1][userID2] == false)
                printf("\nUser %s is not your friend !\n",network->users[userID2].name);
            else
            {
                network->adjacencyMatrix[userID1][userID2] = false;
                printf("\nRemoved %s from your friend's list.\n");
            }
        }
    } 
}

char* generateColor()
{
    char* color = (char*)malloc(7);
    int r = (rand() % 128) + 40; // R: 128-255 (brighter range)
    int g = (rand() % 128) + 40; // G: 128-255 (brighter range)
    int b = (rand() % 128) + 40; // B: 128-255 (brighter range)
    snprintf(color, 7, "#%02X%02X%02X", r, g, b);
    return color;
}

void generateDotFile(SocialNetwork *network, const char *filename) 
{
    FILE *file = fopen(filename, "w");

    // Write the DOT file header
    fprintf(file, "digraph SocialNetwork {\n");
    fprintf(file, "    rankdir=LR;\n"); // Horizontal layout
    fprintf(file, "    graph [size=\"15,20\"];\n");
    fprintf(file, "    edge [dir=single, arrowhead=vee];\n");

    for (int i = 0; i < network->numUsers; i++)
        fprintf(file, "    %s [style=\"filled\", fillcolor=\"%s\"];\n", network->users[i].name, generateColor());

    for (int i = 0; i < network->numUsers; i++) 
    {
        for (int j = 0; j < network->numUsers; j++) 
        {
            if (network->adjacencyMatrix[i][j]) 
                fprintf(file, "    %s -> %s [color=\"%s\"];\n", network->users[i].name, network->users[j].name,generateColor());
        }
    }
    
    fprintf(file, "}\n");
    fclose(file);
}

void showthegraph()
{
    const char *cmdCommand = "dot -Tsvg -O graph.DOT";
    int exitCode = system(cmdCommand);
    // system command
    if (exitCode == 0) 
    {
        printf("Created the GRAPH successfully.\n\n\t\tOpening the GRAPH SVG File...\n\n");
        const char *svgFilePath = "graph.DOT.svg";
        // HINSTANCE is a data type used in Windows programming
        HINSTANCE result = ShellExecute(NULL, "open", svgFilePath, NULL, NULL, SW_SHOWNORMAL);
    }   
    else
        printf("Graph creation failed.\n");
}

void replaceFile(const char *oldFileName, const char *newFileName) 
{
    // Attempt to delete the old file
    if (remove(oldFileName) == 0) 
    {
        // Attempt to rename the new file to the old file's name
        if (rename(newFileName, oldFileName) != 0) 
            perror("Error renaming file");
    } 
    else 
        perror("Error deleting file");   
}

void addOrRemoveFriendsFromFile(char *filename , char *userName,  char *friends , int add)
{
    FILE *file = fopen(filename, "r");
    FILE *tempFile = fopen("tempf.txt", "w");
    char line[100];

    while (fgets(line, sizeof(line), file)) 
    {
        char *username = strtok(line, ",");
        char *friendList = strtok(NULL, "\n");

        if (username != NULL) 
        {
            if (strcmp(username, userName) == 0) 
            {
                if(add==1 && friendList!=NULL)
                {
                    strcat(friendList,",");
                    strcat(friendList,friends);
                    fprintf(tempFile, "%s,%s\n", userName, friendList);
                }
                if(add==1 && friendList==NULL)
                {
                    fprintf(tempFile, "%s,%s\n",userName, friends);
                }
                if(add==0 && friendList!=NULL)
                    fprintf(tempFile, "%s,%s\n", userName, friends); 
            }
            else
            {
                if(friendList==NULL)
                    fprintf(tempFile, "%s,\n", username);
                else
                    fprintf(tempFile, "%s,%s\n", username, friendList);
            }   
        }
    }
    // Close the files
    fclose(file);
    fclose(tempFile);

    // Replace the original file with the temporary file
    replaceFile(filename,"tempf.txt");
}

int addFriends(SocialNetwork *network , char *name)
{
    char friends[256];
    char finalfreinds[256];
    int existingUserID = findid(network,name);
    printf("Enter the user's friends (comma-separated): ");
    scanf("%s", friends);

    char *friend = strtok(friends, ",");
    strcpy(finalfreinds,"");
    while (friend != NULL) 
    {
        
        int friendUserID = findid(network ,friend);
        if (friendUserID != -1) 
        {
            if(network->adjacencyMatrix[existingUserID][friendUserID])
                printf("\nUser %s is already your friend..\n",friend);
            else
            {
                if (strlen(finalfreinds) > 0) 
                    strcat(finalfreinds, ",");

                strcat(finalfreinds, friend);
                addOrRemoveFriendship(network, existingUserID, friendUserID,true);
            }
        }  
        if(friendUserID == -1)
            printf("\nUser %s not found..\n",friend);

        friend = strtok(NULL, ",");
    }
    if(strcmp(finalfreinds,"")!=0)
        addOrRemoveFriendsFromFile("data.txt",name,finalfreinds,1);

    return 1;
}

int followers(SocialNetwork *network , char *name , char *show)
{
    int userID = findid(network,name); 
    int n=0;
    for (int j = 0; j < network->numUsers; j++) 
        if (network->adjacencyMatrix[j][userID])
            n++;

    if(show!=NULL)
    {
        printf("\n\nThe Follower's of %s are :\n\n",name);
        for (int j = 0; j < network->numUsers; j++) 
        {
            if (network->adjacencyMatrix[j][userID])
            {
                printf("\t%d %s\n",n,network->users[j].name);
                n++;
            }
        }  
        printf("\nTotal [Follower's]: %d\n",n); 
    }
    else
        return n;
}


int followingg(SocialNetwork *network , char *name , char *show)
{
    int userID = findid(network,name); 
    int n=1;
    for (int j = 0; j < network->numUsers; j++) 
        if (network->adjacencyMatrix[userID][j])
            n++;
     
    if(show!=NULL)
    {
        n=1;
        printf("\n\nUser %s follow's :\n\n",name);
        for (int j = 0; j < network->numUsers; j++) 
        {
            if (network->adjacencyMatrix[userID][j])
            {
                printf("\t%d %s\n",n,network->users[j].name);
                n++;
            }
        }  
        printf("\nTotal [Following]: %d\n",n-1); 
    }
    else
        return n-1;
}
void getCurrFriendsOfUser(SocialNetwork *network, char *name, char *finalFriends) 
{
    int userID = findid(network, name);
    int n = 1;

    strcpy(finalFriends, "");

    for (int j = 0; j < network->numUsers; j++) 
    {
        if (network->adjacencyMatrix[userID][j] )
        {
            // Append the friend's name to finalFriends
            if (strlen(finalFriends) > 0)
                strcat(finalFriends, ",");
            
            strcat(finalFriends, network->users[j].name);
            n++;
        }
    }
}


int areFriends(SocialNetwork *network,char *namez) 
{
    //system("cls");
    char user1[100];
    char user2[100];
    if(namez!=NULL)
        strcpy(user1,namez);
    else
    {
        printf("\nEnter the first user's name: ");
        scanf("%s", user1);
        
    }
    int userID1 = findid(network,user1);
    if(userID1 == -1)
    {
        printf("\nUser %s doesn't exist.\n", user1);
        return -1;
    }
    printf("\nEnter the second user's name: ");
    scanf("%s", user2);
    int userID2 = findid(network,user2);
    if(userID2 == -1)
    {
        printf("\nUser %s doesn't exist.\n", user2);
        return -1;
    }

    if (network->adjacencyMatrix[userID1][userID2] && network->adjacencyMatrix[userID2][userID1])
        printf("\n\n%s and %s are friends.\n", user1, user2);
    else
        printf("\n\n%s and %s are not friends.\n", user1, user2);

}

void removeUserFromFile(SocialNetwork *network,char *namez) 
{
    system("cls");
    char userName[100];
    strcpy(userName,namez);
    
    int userIndex = findid(network , userName);
    if(userIndex == -1)
        printf("User '%s' not found in the network.\n", userName);
    else
    {
        for (int i = 0; i < network->numUsers; i++) 
        {
            addOrRemoveFriendship(network,userIndex,i,false);
            addOrRemoveFriendship(network,i,userIndex,false);
        }

        // Move the last user in the array to the position of the removed user
        if (userIndex < network->numUsers - 1) 
        {
            network->users[userIndex] = network->users[network->numUsers - 1];
            for (int i = 0; i < network->numUsers; i++) 
            {
                network->adjacencyMatrix[i][userIndex] = network->adjacencyMatrix[i][network->numUsers - 1];
                network->adjacencyMatrix[userIndex][i] = network->adjacencyMatrix[network->numUsers - 1][i];
            }
        }
        network->numUsers--;

        // update the data.txt file
        FILE *file = fopen("data.txt", "r");
        char tempFilename[] = "tempdeluser.txt";
        FILE *tempFile = fopen(tempFilename, "w");
        char line[256]; 

        while (fgets(line, sizeof(line), file) != NULL) 
        {
            int lineLength = strlen(line);
            if (line[lineLength - 1] == '\n')
                line[lineLength - 1] = '\0';

            char *name = strtok(line, ",");
            if (name != NULL && strcmp(name, userName) != 0) 
            {
                fprintf(tempFile, "%s", name);
                char *friendsList = strtok(NULL, ",");
                while (friendsList != NULL) 
                {
                    if (strcmp(friendsList, userName) != 0)
                        fprintf(tempFile, ",%s", friendsList);
                    friendsList = strtok(NULL, ",");
                }
                fprintf(tempFile, "\n");
            }
        }

        fclose(file);
        fclose(tempFile);
        replaceFile("data.txt",tempFilename);
        printf("User '%s' removed from the network.\n", userName);
        
        // update the user_pass.txt filee
        addOrRemoveFriendsFromFile("user_pass.txt",userName,"",-1);
    }
}

void removeFriends(SocialNetwork *network,char *name)
{
    char friends[256];
    char totalfreinds[256];
    int userID = findid(network,name); 

    printf("Enter the list of friends' names to be deleted (comma-separated): ");
    scanf(" %s", friends); // Read the input as a whole line

    int friendID;
    char *friendName = strtok(friends, ",");

    while (friendName != NULL)
    {
        friendID = findid(network, friendName);
        
        if (friendID != -1)
            addOrRemoveFriendship(network,userID,friendID,false);
        else
            printf("Friend '%s' is not found in the network.\n", friendName);
        
        friendName = strtok(NULL, ",");
    }

    getCurrFriendsOfUser(network,name,totalfreinds);
    addOrRemoveFriendsFromFile("data.txt",name,totalfreinds,0);
}

// mainshiittt

bool BFS(SocialNetwork *network,int src, int dest,int dist[MAX_USERS]) 
{
    int n = network->numUsers;
    bool visited[MAX_USERS] = {false};

	int q[n]; //Queue created
	int f=0,r=-1;	//Queue Initialized
	
    visited[src] = true;
    dist[src] = 0;
	q[++r]=src;		//Enqueue src
    
    while(f<=r)	 //while queue is not empty
    {
        int u = q[f++]; //dequeue
        for (int i = 0; i < n; i++) 
        {
            if (network->adjacencyMatrix[u][i] && !visited[i]) 
            {
                visited[i] = true;
                dist[i] = dist[u] + 1;
                q[++r]=i;	 //Enqueue adj vertex
            }
        }
    }
    return visited[dest]; // Return true if a path to the destination exists
}


void findMutualz(SocialNetwork *network , int s)
{
    if(s==-1)
    {
        printf("\nUser does not exits..\n");
        return;
    }
    bool nofreindsz= true;
    int dist[MAX_USERS]={0};
    int n = network->numUsers;

    for (int dest = 0; dest < network->numUsers; dest++) 
    {
        if (dest!=s) 
        {
            if (BFS(network ,s, dest, dist))
                nofreindsz = false;
        }
    }

    printf("Mutuals of %s are:\n", network->users[s].name);
    if(nofreindsz==false)
    {
        for (int i = 0; i < n; i++)
            if (dist[i] > 1)
                printf("\t\t%s = %d\n", network->users[i].name, dist[i]);
    }

    if(nofreindsz==true)
    {
        int threshold=0.1*n;
        printf("No mutual's found for %s\n", network->users[s].name);
        for (int i = 0; i < n; i++)
        {
            if(network->followersOfUser[i]>threshold)
                printf("\t\t%s\n", network->users[i].name);
        }
    }
        
}

void searchbar(SocialNetwork *network,char *loggedin,char *addOrRem) 
{
    char *namez;
    char input[25];
    bool found = false;
    char username[100];
    char choice[25];
    do 
    {
        printf("Enter the username to search (or 'q' to exit): ");
        scanf("%s", input);

        if (strcmp(input, "q") == 0) 
            break; 

        found = false;  

        for (int i = 0; i < network->numUsers; i++) 
        {
            strcpy(username,network->users[i].name);
            if (strstr(strlwr(username), strlwr(input)) != NULL) 
            {
                found = true;
                printf("\t\t%s\n", network->users[i].name);
            }
        }

        if (!found)
            printf("\n%s user does not exist. Try another name.\n", input);
        
        if(found==true && loggedin!=NULL)
        {
            if(strcmp(addOrRem,"add")==0)
            {
                printf("\nDo u want to add friends (y/n) : ");
                scanf("%s",choice);
                if(strcmp(choice,"y")==0)
                    addFriends(network,loggedin);
                
            }
            if(strcmp(addOrRem,"remove")==0)
            {
                printf("\nDo u want to remove friends (y/n) : ");
                scanf("%s",choice);
                if(strcmp(choice,"y")==0)
                    removeFriends(network,loggedin);
        
            }
           
        }

    }while (1);
}

// MAINN CONTROL FUNCTIONS

void functionsofuser(SocialNetwork *network,char *userlogedin)
{
    int choice;
    char ans[10];
    
    do {
        printf("\n\t\t\tWelcome %s !!!\n\n",userlogedin);
        printf("\n\n\n");
        printf("1. Show the graph\n");
        printf("2. Search for someone on the network.\n");
        printf("3. Show my Following\n");
        printf("4. Show my Follower's\n");
        printf("5. Find my Influence Score \n");
        printf("6. Add friend's\n");
        printf("7. Remove friend's\n");
        printf("8. Show my Mutual's.\n");
        printf("9. Check if you're buddies with someone.\n");
        printf("10. Delete my Account\n");
        printf("11. Back\n");
        printf("\n\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) 
        {
            case 1:
                // Show the Graph
                system("cls");
                generateDotFile(network, "graph.DOT");
                showthegraph();
                break;
            case 2:
                searchbar(network,NULL,NULL);
            case 3:
                followingg(network,userlogedin,"show");
                break;
            case 4:
                followers(network,userlogedin,"show");
                break;
            case 5:
                calculateNoOfFollowrs(network);
                friendscoreOfUser(network,userlogedin);
                break;
            case 6:
                searchbar(network,userlogedin,"add");
                //addFriends(network,userlogedin);
                break;
            case 7:
                searchbar(network,userlogedin,"remove");
                //removeFriends(network,userlogedin);
                break;
            case 8:
                // find mutaulzz
                findMutualz(network , findid(network,userlogedin));
                break;
            case 9:
                // check friednshipp
                searchbar(network,NULL,NULL);
                areFriends(network,userlogedin);
                break;
            case 10:
                // delete user's account
                printf("Are u sure (Yes/No) : \n");
                scanf("%s[^\n]",ans);
                if(strcmp(ans,"Yes")==0)
                    removeUserFromFile(network,userlogedin);
                else
                    printf("\nUser Account is not deleted..Goin back to main menu");
                menu(network);
                break;
            case 11:
                printf("Returning to Main Menu.\n");
                menu(network);
                break;
            default:
                printf("Invalid choice. Please select a valid option.\n");
        }
    }while (choice<12);
}

void admin(SocialNetwork *network)
{
    int choice;
    char ans[25];
    char name[25];
    do {
        printf("\n\n\n");
        printf("1. Show the Graph\n");
        printf("2. Show the Matrix [of all users]\n");
        printf("3. Display all the USER's along with their ID's\n");
        printf("4. Search an User.\n");
        printf("5. Add an USER to the network\n");
        printf("6. Remove an USER from the network\n");
        printf("7. Find Mutual's of an USER.\n");
        printf("8. Show the Infulence Metric of the Network\n");
        printf("9. Check if 2 users are friends\n");
        printf("10. Find the follower list of an USER\n");
        printf("11. Find the followin list of an USER\n");
        printf("12. Open the \"data.txt\" file\n");
        printf("13. Open the \"user_pass.txt\" file\n");
        printf("14. Back\n");
        printf("\n\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) 
        {
            case 1:
                // Show the Graph
                system("cls");
                generateDotFile(network, "graph.DOT");
                showthegraph();
                break;
            case 2:
                // Show the Matrix [of all users]
                displaySocialNetwork(network);
                break;
            case 3:
                // Display all the USER's along with their ID's
                system("cls");
                displayUsersWithIDs(network);
                break;
            case 4:
                // searchh
                searchbar(network,NULL,NULL);
                break;
            case 5:
                // Add an USER to the network
                addNewUser(network);
                break;
            case 6:
                // Remove an USER from the network
                printf("Enter the user's name to be deleted: ");
                scanf("%s", name);
                printf("Are u sure (Yes/No) : \n");
                scanf("%s[^\n]",ans);
                if(strcmp(ans,"Yes")==0)
                    removeUserFromFile(network,name);
                else
                    printf("\nUser Account is not deleted..Goin back to main menu");
                break;
            case 7:
                //find mutual
                printf("Enter the user's name whose mutual's u wanna find outt :\n");
                scanf("%s", name);
                findMutualz(network , findid(network,name));
                break;
            case 8:
                //infleunce metric
                calculateNoOfFollowrs(network);
                friendscoreOfallUsers(network);
                break;
            case 9:
                //check freidnshipp
                //searchbar(network,NULL,NULL);
                areFriends(network,NULL);
                break;
            case 10:
                // follower list
                printf("Enter the user's name whose follower's you want to find :\n");
                scanf("%s", name);
                followers(network,name,"show");
                break;
            case 11:
                // following list
                printf("Enter the user's name whose following list you want to find :\n");
                scanf("%s", name);
                followingg(network,name,"show");
                break;
                break;
            case 12:
                // to open data.txt
                opendatafile();
                break;
            case 13:
                // to open password file
                openpassfile();
                break;
            case 14:
                printf("Returning to Main Menu.\n");
                menu(network);
                break;
            default:
                printf("Invalid choice. Please select a valid option.\n");
        }
    }while (choice<15);
}

void menu(SocialNetwork *network)
{
    int choice;
    char *namez;
    do
    {
    printf("\nMain Menu:\n");
    printf("1. Admin\n");
    printf("2. New User\n");
    printf("3. Existing User\n");
    printf("4. QUIT\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice) 
    {
        case 1:
            checkadmin(network);
            break;
        case 2:
            if(addNewUser(network)!=0)
                checkuserIsExisting(network);
            break;
        case 3:
            readuserpass(network);
            checkuserIsExisting(network);
            break;
        case 4:
            printf("\n\tExitingg...\t\n");
            exit(0);
            break;
        default:
            printf("Invalid choice. Please select a valid option.\n");
    }
    } while (choice<5);
}

int main() 
{
    SocialNetwork network;

    initSocialNetwork(&network);
    readData(&network, "data.txt");
    readuserpass(&network);
    calculateNoOfFollowrs(&network);
    menu(&network);

    return 0;
}