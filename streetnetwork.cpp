#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <cstdlib>
using namespace std;

struct Apartment{
    string name;
    int max_bandwidth;
    struct Apartment* next;
    struct Flat* flatlist; 
    int flatcount; 
    int sumofinitials; //this value holds flats' initial bandwidth value sum
};

//this function makes new apartment by allocation,assigns aptname and bandwidth values,parameter count is used for total apartment numbers
Apartment* newapartment(int* count,string aptname, int bandwidth){
    (*count)++;
    Apartment* p = new Apartment;
    p->name = aptname;
    p->max_bandwidth = bandwidth;
    p->flatlist = NULL; //initial flatlist is Null
    p->flatcount = 0;
    p->sumofinitials=0;
    return p;
}

struct Flat{
    int id;
    int initial_bandwidth;
    int is_empty;
    struct Flat* next;
    struct Flat* prev;
};

/*this function makes new flat by allocation, parameters aptmaxbandwidth and suminitial are used to compare values and assign
initial bandwidth value according to them */

struct Flat* new_flat(int initialband,int fid,int aptmaxbandwidth,int* suminitial){
    Flat* p = new Flat;
    p->id = fid;
    p->is_empty = 0;
    if (aptmaxbandwidth == *suminitial){ //if given flats'initial bandwidth values are equal to maxbandwidth, assign 0 to new flat
        p->initial_bandwidth = 0;
        p->is_empty = 1;
    }
    else if(aptmaxbandwidth-(*suminitial) < initialband){
        p->initial_bandwidth = aptmaxbandwidth-(*suminitial); //if there isn't enough room for new flat's initial bandwidth, just assign remaining bandwidth value
    }
    else{
        p->initial_bandwidth = initialband;
    }
    (*suminitial)+=p->initial_bandwidth;
    
    return p;
}

/* add_apartment function adds apartments to given position and calls newapartment function by 
using given parameters. It controls if given position effects head/tail nodes,and act according to that
*/
void add_apartment(struct Apartment** head,struct Apartment** tail, string aptname,string position,int bandwidth,int* count ){
    Apartment* p = *head; 
    string arglist[2];
    if (position!= "head") { //this part is used to divide before_X / after_X string
        stringstream ss(position);
        string word;
        int i=0;
        while(!ss.eof()) {
            getline(ss,word,'_');
            arglist[i] = word;
            i++;
        } //arglist[0] holds before/after
        //arglist[1] holds the node 
    }
    Apartment* newone = newapartment(count,aptname, bandwidth); //makes new apartment
    
    if (*head==NULL){
        *head =*tail = newone;
    }
    else if ((arglist[0]=="before" && arglist[1] == (*head)->name) || (position =="head")){
        //if the position is head or before_X such that X is head, then we have to change real head pointer
        (*tail)->next = newone;
        newone->next = *head ;
        *head = newone; //to change completely
    }
    else if (arglist[0]=="after" && arglist[1] == (*tail)->name){
        //if the X node in 'after_X' is tail, then we have to change real tail pointer
        newone->next = *head;
        (*tail)->next = newone;
        *tail = newone;
    }
    else if(arglist[0]=="before"){
        while (p->next->name!= arglist[1]){ 
            p = p->next; 
        }//when the loop ends, p is the node before X node in 'before_X' position
        Apartment* nextone = p->next;
        p->next= newone;
        newone->next = nextone;
    }
    else if(arglist[0]=="after"){
        while(p->name!= arglist[1]){
            p = p->next;
        }//when the loop ends, p is the X node in 'after_X' position
        Apartment* nextone = p->next;
        p->next = newone;
        newone->next = nextone;
    }
}

/* add_flat function makes new flat by calling newflat with given paremeters, adds new flat to given apartment's flatlist's given index.
If the flatlist is null or given index is 0, it does different operations. If it's not, it basically changes prev and next nodes of new flat's given index
*/
void add_flat(struct Apartment* head,string aptname, int index, int initialband, int flatid,int aptcount){
    Apartment* aptflat = head; //the apartment that we're gonna add flat
    Flat* nextflat;
    Flat* prevflat; //flat before the given index
    for (int i= 0; i<aptcount;i++){
        if (aptflat->name== aptname){
            Flat* newflat = new_flat(initialband,flatid,(aptflat->max_bandwidth),&(aptflat->sumofinitials)); //creates a new flat by using given parameters
            aptflat->flatcount++; 
            
            if(aptflat->flatlist==NULL){
                aptflat->flatlist = newflat; //if flatlist is null, assign the new flat as head of flatlist (aptflat->flatlist indicates head of flatlist)
                newflat->next = NULL;
            }
            else if (index==0){ //if given index is 0, that means head of flatlist will change,this condition is for that
                nextflat = (aptflat->flatlist);
                newflat->next = nextflat;
                aptflat->flatlist = newflat;
                nextflat->prev = newflat;
            }
            else{
                Flat* flatlist = aptflat->flatlist; //holds initial value of flatlist so that it will not change after the adding part ends
                for (int i= 0; i<index-1;i++){
                    aptflat->flatlist = aptflat->flatlist->next;
                }
                prevflat = aptflat->flatlist;
                newflat->next = prevflat->next;
                prevflat->next = newflat;
                newflat->prev = prevflat;
                if (newflat->next!=NULL){
                    newflat->next->prev = newflat;
                }
                aptflat->flatlist = flatlist; //it turned back to beginning of flatlist
            }
            break; //no need to continue searching if the apartment is found
        }
        aptflat = aptflat->next;
    }
}

//this function deletes flats from flatlist
void free_flatlist(Apartment* apt){
    Flat* f = apt->flatlist;
    if (f!=NULL){
        apt->flatlist = apt->flatlist->next;
        delete f;
        free_flatlist(apt);
    }
}

//this function deletes the given node and its flatlist nodes
void free_apt(Apartment* head){
    free_flatlist(head);
    delete head;
}

//this function removes apartment by searching name of apartment
struct Apartment* remove_apartment(struct Apartment** head,struct Apartment** tail,string aptname,int* aptcount){
    Apartment* p = *head;
    Apartment* temp; //holds the apartment node that is before the apartment that will be removed
    Apartment* aptremove; //holds the apartment that will be deleted, it's the parameter for free_apt
    for (int i=0;i<(*aptcount);i++){
        if (p->name == aptname){
            if((*head)->name==aptname){ //if the given apartment is head, we have to change head node
                aptremove = *head;
                (*tail)->next = (*head)->next;
                *head = (*head)->next;
                free_apt(aptremove);
            }
            else if((*tail)->name==aptname){ //if the given apartment is tail, we have to change tail node
                aptremove = *tail;
                temp->next = (*tail)->next;
                *tail = temp;
                free_apt(aptremove);
            }
            else{
                aptremove = p;
                temp->next = p->next;
                free_apt(aptremove);

            }
            (*aptcount)-=1;
            break;
        }
        temp = p;
        p = p->next;
    }
    if (aptcount==0){
        return NULL;
    }
    else{
        return *head;
    }
}

//make_flat_empty function finds flat that's id is flatid, and changes it's empty flag to 1
void make_flat_empty(Apartment* head,string aptname, int flatid, int aptcount){
    Apartment* p = head;
    for (int i= 0; i< aptcount;i++){
        if (p->name==aptname){
            Flat* flat = p->flatlist;
            for (int j = 0; j< p->flatcount; j++){
                if (flat->id==flatid){
                    flat->initial_bandwidth = 0;
                    flat->is_empty = 1;
                    break;
                }
                flat = flat->next;
            }
            break;
        }
        p = p->next;
    }
}

//find_sum_of_max_bandwidth function sums apartment's maxbandwidth values
int find_sum_of_max_bandwidth(Apartment* head,int aptcount){
    int sum= 0;
    Apartment *p = head;
    if (head == NULL){
        return 0;
    }
    else{
        for (int i= 0; i<aptcount;i++){
            sum+= p->max_bandwidth;
            p = p->next;
        }
        return sum;
    }
}

/*merge_two_apartments function appends flatlist of apt2 to apt1, adds maxbandwidth and flatcount values of apt2 
to apt1, and deletes apt2 by calling remove_apartment function
*/
Apartment* merge_two_apartments(Apartment** head,Apartment** tail, string apt1, string apt2,int* aptcount){
    Apartment* a1 = *head; //this pointer will hold apartment1
    Apartment* a2 = *head; //this pointer will hold apartment2

    for (int i= 0; i< *aptcount; i++){ //used to find apt1
        if (a1->name== apt1){
            break;
        }
        a1 = a1->next;
    }
    for (int i= 0; i< *aptcount; i++){ //used to find apt2
        if (a2->name== apt2){
            break;
        }
        a2 = a2->next;
    }
    a1->max_bandwidth+= a2->max_bandwidth;
    Flat* lastflat = a1->flatlist;
    if (a1->flatlist!=NULL){
        for (int i= 0; i<(a1->flatcount)-1;i++){
            lastflat = lastflat->next;
        } //end of the loop lastflat is the last flat of apt1 flatlist 
        lastflat->next = a2->flatlist;
        if (a2->flatlist!=NULL){
            a2->flatlist->prev = lastflat;
        }
    }
    else{
        a1->flatlist = a2->flatlist;
    }
         
    a1->flatcount+=a2->flatcount;
    a2->flatlist = NULL;
    remove_apartment(head,tail,apt2,aptcount);
    return *head;
}

//list_apartments lists apartments and their flats
void list_apartments(ofstream &outputfile, Apartment* head,int aptcount){
    Apartment* p = head;
    if(head==NULL){
        outputfile<<"There is no apartment"<<endl;
    }
    else{
        for (int i= 0; i<aptcount;i++){
            outputfile<<p->name<<"("<<p->max_bandwidth<<")\t";
            Flat* flat = p->flatlist;
            for (int j= 0; j<p->flatcount;j++){
                if (p->flatlist != NULL){
                    outputfile<<"Flat"<<p->flatlist->id<<"("<<p->flatlist->initial_bandwidth<<")\t";
                    p->flatlist=p->flatlist->next;
                }
            }
            p->flatlist = flat; 
            p = p->next;
            outputfile<<endl;
        }
    }
}

/* relocate_flats_to_same_apartments function firstly finds flat that it's id=num, then move it from its apartment by changing prev/next node connections.
After finding and removing connections of flat, it finds the given apartment in linkedlist, and the flat that it's id corresponds to flatidshift.
Then, it adds the moving flat before the flatidshift by making correct connections.
*/
void relocate_flats_to_same_apartments(Apartment* head, Apartment* tail,int aptcount, string newapartment,int flatidshift,int num){
    Apartment* p = head;
    Apartment* h= head;
    Flat* flat_move;
    //this part is for removing the connections of flat that will be relocated
    for (int i= 0; i<aptcount;i++){
        Flat* initialflatlist = p->flatlist;
        for (int j= 0; j<p->flatcount;j++){
            if (initialflatlist != NULL && p->flatlist != NULL){
                if(p->flatlist->id == num){ //that means we found the flat and its apartment
                    flat_move = p->flatlist; //flat that will be relocated
                    
                    if(flat_move->id==initialflatlist->id){ // if the flat is the head of flatlist, then we change the head
                        initialflatlist = flat_move->next;
                        if(flat_move->next !=NULL){
                            flat_move->next->prev= NULL; 
                        }  
                    }
                    
                    else{
                        flat_move->prev->next = flat_move->next;
                        if(flat_move->next!=NULL){
                            flat_move->next->prev = flat_move->prev;
                        }
                    }
                    p->max_bandwidth-=flat_move->initial_bandwidth;
                    p->flatlist = initialflatlist; //to return the beginning of flatlist
                    p->flatcount--; //as we moved the flat, we decrease number of flats
                    break;
                }
            p->flatlist = p->flatlist->next;
            }
        }
        p->flatlist = initialflatlist;
        p = p->next;
    }

    //this part is for adding the flat to given apartment in given position
    for (int i= 0; i<aptcount;i++){
        if(h->name == newapartment){
            Flat* hflat = h->flatlist;
            for (int j= 0; j<h->flatcount;j++){
                if(h->flatlist->id==flatidshift){
                    if(flatidshift==hflat->id){ //that means flatidshift is the head of apartment's flatlist, that means we have to change the head
                        flat_move->next = h->flatlist;
                        h->flatlist->prev= flat_move;
                        hflat = flat_move; //new head is flat_move
                    }
                    else{
                        flat_move->next=h->flatlist;
                        flat_move->prev = h->flatlist->prev;
                        h->flatlist->prev->next = flat_move;
                        h->flatlist->prev = flat_move;
                    }
                    h->flatcount++; //as we added the relocated flat, we increase number of flats
                    h->max_bandwidth+=flat_move->initial_bandwidth;
                    h->flatlist = hflat;
                    break;
                }
                h->flatlist = h->flatlist->next;
            }
        }
        h = h->next;
    }
}

//delete__all_the_nodes function deletes all apartments and its flats
void delete_all_the_nodes(Apartment** head,Apartment** tail){
    Apartment* p = *head;
    if(*head==*tail){
        free_apt(*head);
    }
    else{
        *head = p->next;
        free_apt(p);
        delete_all_the_nodes(head,tail);
    }

}


int main(int argc, char *argv[]){
    int aptcount =0;
    struct Apartment* head = NULL;
    struct Apartment* tail = NULL;

    string line;
    fstream myfile;
    myfile.open(argv[1],ios::in);
    ofstream outputfile;
    outputfile.open(argv[2]);
    
    while(getline(myfile,line)){
        
        if (line[line.length()-1]=='\r'){
            line.erase(line.length()-1);
        }
        string commands[5]; //holds the strings in every command
        stringstream ss(line);
        string word;
        int i = 0;
        while(!ss.eof()){
            getline(ss,word,'\t');
            commands[i] = word;
            i++;
        }
        if (commands[0] == "add_apartment"){
            add_apartment(&head,&tail,commands[1],commands[2],atoi(commands[3].c_str()),&aptcount);
        }
        else if( commands[0] == "add_flat"){
            add_flat(head,commands[1],atoi(commands[2].c_str()),atoi(commands[3].c_str()),atoi(commands[4].c_str()),aptcount);
        }
        else if( commands[0] == "remove_apartment"){
            remove_apartment(&head,&tail,commands[1],&aptcount);
        }
        else if( commands[0] == "merge_two_apartments"){
            merge_two_apartments(&head,&tail,commands[1],commands[2],&aptcount);
        }
        else if( commands[0] == "find_sum_of_max_bandwidths"){
            outputfile<<"sum of bandwidth: "<<find_sum_of_max_bandwidth(head, aptcount)<<endl;
            outputfile<<endl;
        }
        else if (commands[0] == "make_flat_empty"){
            make_flat_empty(head,commands[1],atoi(commands[2].c_str()),aptcount);
        }
        else if (commands[0] == "relocate_flats_to_same_apartment"){
            string st = commands[3];
            st = st.substr(1,(st.size())-2); //it is used to get rid of [] in given command
            stringstream ss(st);
            string w;
            while(!ss.eof()){ // this loop splits given flatlist by ',', and then calls relocate function for every number
                getline(ss,w,',');
                relocate_flats_to_same_apartments(head,tail,aptcount,commands[1],atoi(commands[2].c_str()),atoi(w.c_str()));
            }
        }
        else if (commands[0]=="list_apartments"){
            list_apartments(outputfile,head,aptcount);
            outputfile<<endl;
        }
    }
    
    
    myfile.close();
    outputfile.close();
    delete_all_the_nodes(&head,&tail);
    
    return 0;
}
