#include<stdio.h>
#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/time.h>
#include<pthread.h>
#include<libgen.h>
#include<arpa/inet.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


using namespace rapidjson;
using namespace std;

unsigned long long reqID=0;
unsigned long long currentclientuid=1;
unsigned long long currentslaveuid=1;
int numberOfSlaveServers;


Document document;

int globalclientport=3000;
int globalslaveport=10000;


map <int,string> slaveUidToipport;
map <int,string> clientUidToipport;

typedef struct{
  int sock2;
  char  *structBuffer=new char[100];
}parameter;

void
error(const char *msg){
    perror(msg);
    exit(1);
}

string prepareREGISTERACKCLIENTmessageinjson(int pvalue,int port)//pvalue=0 failed registration
{
    string str=" { \"reqid\" :"+ to_string(reqID) +", \"reqtype\" : \"acknowledgeclientreq\", \"registered\" : "+to_string(pvalue)+", \"port\" : "+to_string(port)+" } ";
    reqID++;
    return str;
}

string prepareREGISTERACKSLAVEmessageinjson(int qvalue,int port)//pvalue=0 failed registration
{
    string str=" { \"reqid\" :"+ to_string(reqID) +", \"reqtype\" : \"acknowledgeslavereq\", \"registered\" : "+to_string(qvalue)+", \"port\" : "+to_string(port)+" } ";
    reqID++;
    return str;
}

void jsonstringtodocument(string jsonstring)
{


    cout<<jsonstring<<endl;
#if 0
    // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
    if (document.Parse(json).HasParseError())
        return 1;
#else
    // In-situ parsing, decode strings directly in the source string. Source must be string.
    char buffer[jsonstring.length()];
    memcpy(buffer, jsonstring.c_str(), jsonstring.length());
    cout<<buffer<<endl;

    if (document.ParseInsitu(buffer).HasParseError())
        return;
#endif

    assert(document.IsObject());

    assert(document.HasMember("clienthostid"));
    assert(document["clienthostid"].IsString());


    printf("\nParsing to document succeeded.\n");


    cout<<"This is clienthostid:"<<document["clienthostid"].GetString()<<endl;


    return;
}


int hashfunction(string key)
{
    int modval=key.length()%4;
    int n=key.length()-modval;
    string hashedString;
    for(int i=0;i<n;i+=4){
        int num1=(int)key[i];
        int num2=(int)key[i+1];
        int num3=(int)key[i+2];
        int num4=(int)key[i+3];
        int num=(num1+num2+num3+num4)%2;
        hashedString+=to_string(num);
    }
    int num=0;
    for(int i=n;i<key.length();i++){
        num+=key[i];
    }
    if(modval!=0){
        num=num%2;
        hashedString+=to_string(num);
    }
    return stoi(hashedString, nullptr, 2);

}

void*
connectSlave(void *node){
    int port=*((int *)node)-1;
    cout<<"Port"<<port<<endl;
    struct sockaddr_in server, client;
    int sock1,sock2;
    sock1=socket(AF_INET, SOCK_STREAM, 0);
    if(sock1<0)
        error("Error Creating Socket");
    bzero((char *)&server, sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    server.sin_addr.s_addr=INADDR_ANY;
    if(bind(sock1, (struct sockaddr*)&server, sizeof(server))<0)
        error("Error Binding");
    if(listen(sock1, 1)<0)
        error("Error Listen");
    socklen_t clientLen = sizeof(client);
    pthread_t threadid;
    sock2=accept(sock1, (struct sockaddr*)&client,&clientLen);
    cout<<"Connection done with slave at port "<<port<<endl;

   /* while(true)
    {
        char queryarray[300];
        bzero(queryarray, 300);
        recv(sock2, queryarray, 300, 0);
        string querystring(queryarray);

        if (document.ParseInsitu(queryarray).HasParseError())
        {
            cout<<"Error Parsing Query"<<endl;
        }
           // return;
        else{
        assert(document.IsObject());
        assert(document.HasMember("reqtype"));
        assert(document["reqtype"].IsString());
        //cout<<"T"<<document["reqtype"].GetString()<<"X"<<endl;
        //if(document["reqtype"].GetString()=="putreq")
        if(strcmp(document["reqtype"].GetString(),"putreq")==0)
            cout<<"This is Put Request"<<endl;
        else if(strcmp(document["reqtype"].GetString(),"getreq")==0)
            cout<<"This is Get Request"<<endl;
        else if(strcmp(document["reqtype"].GetString(),"delreq")==0)
            cout<<"This is Del Request"<<endl;
        else if(strcmp(document["reqtype"].GetString(),"byereq")==0)
            {cout<<"Connection ends"<<endl;close(sock2);break;}

        }
    }*/
}

void*
connectClient(void *node){
    int port=*((int *)node)-1;
    cout<<"Port"<<port<<endl;
    struct sockaddr_in server, client;
    int sock1,sock2;
    sock1=socket(AF_INET, SOCK_STREAM, 0);
    if(sock1<0)
        error("Error Creating Socket");
    bzero((char *)&server, sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(port);
    server.sin_addr.s_addr=INADDR_ANY;
    if(bind(sock1, (struct sockaddr*)&server, sizeof(server))<0)
        error("Error Binding");
    if(listen(sock1, 1)<0)
        error("Error Listen");
    socklen_t clientLen = sizeof(client);
    pthread_t threadid;
    sock2=accept(sock1, (struct sockaddr*)&client,&clientLen);


    while(true)
    {
        char queryarray[300];
        bzero(queryarray, 300);
        recv(sock2, queryarray, 300, 0);
        string querystring(queryarray);

        if (document.ParseInsitu(queryarray).HasParseError())
        {
            cout<<"Error Parsing Query"<<endl;
        }
           // return;
        else{
        assert(document.IsObject());
        assert(document.HasMember("reqtype"));
        assert(document["reqtype"].IsString());
        //cout<<"T"<<document["reqtype"].GetString()<<"X"<<endl;
        //if(document["reqtype"].GetString()=="putreq")
        if(strcmp(document["reqtype"].GetString(),"putreq")==0)
            {
                cout<<"This is Put Request"<<endl;

                string key(document["key"].GetString());

                int hashedkey=hashfunction(key);

                int slaveServerNumber=((hashedkey%numberOfSlaveServers)+1);

                cout<<slaveServerNumber<<endl;

                string ipPortCombination=slaveUidToipport[slaveServerNumber];

                string ipaddress=ipPortCombination.substr(0, ipPortCombination.find(":"));

                string portaddress=ipPortCombination.substr(ipPortCombination.find(":")+1);

                cout<<"IP "<<ipaddress<<" PORT "<<portaddress<<endl;


            }

        else if(strcmp(document["reqtype"].GetString(),"getreq")==0)
            {
                cout<<"This is Get Request"<<endl;

                string key(document["key"].GetString());

                int hashedkey=hashfunction(key);

                int slaveServerNumber=((hashedkey%numberOfSlaveServers)+1);

                cout<<slaveServerNumber<<endl;

                string ipPortCombination=slaveUidToipport[slaveServerNumber];

                string ipaddress=ipPortCombination.substr(0, ipPortCombination.find(":"));

                string portaddress=ipPortCombination.substr(ipPortCombination.find(":")+1);

                cout<<"IP "<<ipaddress<<" PORT "<<portaddress<<endl;


            }
        else if(strcmp(document["reqtype"].GetString(),"delreq")==0)
            {
                cout<<"This is Del Request"<<endl;

                string key(document["key"].GetString());

                int hashedkey=hashfunction(key);

                int slaveServerNumber=((hashedkey%numberOfSlaveServers)+1);

                cout<<slaveServerNumber<<endl;

                string ipPortCombination=slaveUidToipport[slaveServerNumber];

                string ipaddress=ipPortCombination.substr(0, ipPortCombination.find(":"));

                string portaddress=ipPortCombination.substr(ipPortCombination.find(":")+1);

                cout<<"IP "<<ipaddress<<" PORT "<<portaddress<<endl;


            }
        else if(strcmp(document["reqtype"].GetString(),"byereq")==0)
            {cout<<"Connection ends"<<endl;close(sock2);break;}

        }
    }

}

//CACHING STARTS
#define CACHE_SIZE 4

map<long,int> cacheMap;

void newRequest(long cacheArray[],long ele,int *currentIndex,int *isFull){


map<long,int>::iterator it;

it=cacheMap.find(ele);

if(it!=cacheMap.end()){

//cout<<"HII";
cacheMap.find(ele)->second=1;

cout<<"\n HIT: "<<cacheMap.find(ele)->second<<"\n";

}
else
{
        if(*isFull>0){
                cacheArray[*currentIndex]=ele;
                *currentIndex=(*currentIndex%CACHE_SIZE)+1;
                cacheMap.insert(pair<long,int>(ele,0));
                 *isFull=*isFull-1;
                  cout<<*currentIndex<<" "<<*isFull<<endl;
        }
        else{

                it=cacheMap.find(cacheArray[(*currentIndex)%CACHE_SIZE]);
                while(it->second==1){

		it->second=0;
                *currentIndex=((*currentIndex+1)%CACHE_SIZE);
                it=cacheMap.find(cacheArray[*currentIndex]);

                }
                if(it!=cacheMap.end())
                   cacheMap.erase(it);
                    cacheMap.insert(pair<long,int>(ele,0));
                cacheArray[(*currentIndex)%CACHE_SIZE]=ele;
                *currentIndex=(*currentIndex+1)%CACHE_SIZE;
        }

}

}

//CACHING ENDS
int
main(int argc,char *argv[]){
    //cache starts
    long cacheArray[CACHE_SIZE];

    int currentIndex=0;
    int isFull=CACHE_SIZE;

    //cache ends
    cout<<"Enter number of slave servers:"<<endl;
    cin>>numberOfSlaveServers;


    char *port;
    if(argc!=3){
        error("Not Enough Arguments");
    }
    port=argv[2];
//starting server port and listening for connection

struct sockaddr_in server, client;
int sock1,sock2;
sock1=socket(AF_INET, SOCK_STREAM, 0);
if(sock1<0)
    error("Error Creating Socket");
bzero((char *)&server, sizeof(server));
server.sin_family=AF_INET;
server.sin_port=htons(atoi(port));
server.sin_addr.s_addr=INADDR_ANY;
if(bind(sock1, (struct sockaddr*)&server, sizeof(server))<0)
    error("Error Binding");
if(listen(sock1, 1)<0)
    error("Error Listen");
socklen_t clientLen = sizeof(client);
pthread_t threadid;
while((sock2=accept(sock1, (struct sockaddr*)&client,&clientLen))>0)
{
    parameter node;
    char abuffer[200];
    char nodeType;
    bzero(abuffer, 200);
    recv(sock2, abuffer, 200, 0);
    string buffer(abuffer);


    //char buffer[jsonstring.length()];
    //memcpy(buffer, jsonstring.c_str(), jsonstring.length());
    //cout<<buffer<<endl;

    int pvalue,qvalue;
    if (document.ParseInsitu(abuffer).HasParseError())
    {
        cout<<"Error Parsing Document"<<endl;
        pvalue=0;
        qvalue=0;
    }
       // return;
    else if(strcmp(document["reqtype"].GetString(),"registerclientreq")==0)
    {
    assert(document.IsObject());

    assert(document.HasMember("clienthostid"));
    assert(document["clienthostid"].IsString());


    printf("\nParsing to document succeeded.\n");


    pvalue=currentclientuid;
    currentclientuid++;

    cout<<"This is clienthostid:"<<document["clienthostid"].GetString()<<endl;


    char result[100];
    strcpy(result,document["clienthostid"].GetString());
    strcat(result,":");
    strcat(result,(to_string(document["clientport"].GetInt())).c_str());
    string result2(result);
    clientUidToipport[pvalue]=result2;


    string ackstring=prepareREGISTERACKCLIENTmessageinjson(pvalue,globalclientport++);
    cout<<ackstring<<endl;
    send(sock2,ackstring.c_str(),300,0);

    /*size_t a=buffer.find("#");
    nodeType=(char)*(buffer.substr(0,a).c_str());
    cout<<nodeType<<endl;
    buffer=buffer.substr(a+1);
    cout<<buffer<<endl;
    node.sock2=sock2;
    strcpy(node.structBuffer,buffer.c_str());
    //node.structBuffer=(char *)buffer.c_str();
    if(nodeType!='0'){
        */
    if(pthread_create(&threadid,NULL,connectClient,(void*)&globalclientport)<0){
      error("Thread error");
    }

    }

    else if(strcmp(document["reqtype"].GetString(),"registerslavereq")==0)
    {

    assert(document.IsObject());

    assert(document.HasMember("slavehostid"));
    assert(document["slavehostid"].IsString());


    printf("\nParsing to document succeeded.\n");


    qvalue=currentslaveuid;
    currentslaveuid++;

    cout<<"This is slavehostid:"<<document["slavehostid"].GetString()<<endl;
    cout<<"This is slaveport:"<<document["slaveport"].GetInt()<<endl;

    char result[100];
    strcpy(result,document["slavehostid"].GetString());
    strcat(result,":");
    strcat(result,(to_string(document["slaveport"].GetInt())).c_str());
    string result2(result);
    cout<<result2<<endl;
    slaveUidToipport[qvalue]=result2;

    string ackstring=prepareREGISTERACKSLAVEmessageinjson(qvalue,globalslaveport++);
    cout<<ackstring<<endl;
    send(sock2,ackstring.c_str(),300,0);





    /*size_t a=buffer.find("#");
    nodeType=(char)*(buffer.substr(0,a).c_str());
    cout<<nodeType<<endl;
    buffer=buffer.substr(a+1);
    cout<<buffer<<endl;
    node.sock2=sock2;
    strcpy(node.structBuffer,buffer.c_str());
    //node.structBuffer=(char *)buffer.c_str();
    if(nodeType!='0'){
        */
    if(pthread_create(&threadid,NULL,connectSlave,(void*)&globalslaveport)<0){
      error("Thread error");
    }

    }

    else
    {
        cout<<"Error in received request from neither a slave nor a client"<<endl;
    }
    //}
    /*else{
        if(pthread_create(&threadid,NULL,connectClient,(void*)&node)<0){
        error("Thread error");
        }
    }*/
}
return 0;
}
