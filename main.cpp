#include<bits/stdc++.h>
#include<windows.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
using namespace std;

sem_t message_mutex,buffer_mutex,sem_empty,sem_full,writing_mutex;
const int N=3;
int i=0,n;
int buffer[N];
int arr[1000];
int cnt;
pthread_t p[N],monitorthread,collectorthread;

void* Message(void *arg){
    //Sleep(rand()%7);
    //sleep(rand()%3);
    this_thread::sleep_for(chrono::milliseconds(rand()%9));

    int number=*(int*)arg;
    sem_wait(&writing_mutex);
    cout<<"Counter thread "<<number<<": received a message\n";
    sem_post(&writing_mutex);

    int msgmtxvalue;
    sem_getvalue(&message_mutex,&msgmtxvalue);
    if(msgmtxvalue<=0){
        sem_wait(&writing_mutex);
        cout<<"Counter thread "<<number<<": waiting to write\n";
        sem_post(&writing_mutex);
    }

    sem_wait(&message_mutex);
    cnt++;
    sem_wait(&writing_mutex);
    cout<<"Counter thread "<<number<<": now adding to counter, counter value="<<cnt<<"\n";
    sem_post(&writing_mutex);
    sem_post(&message_mutex);

}

void* Monitor(void *arg){
    while(1){
    //Sleep(rand()%5+1);
    //sleep(rand()%3);
    this_thread::sleep_for(chrono::milliseconds(rand()%5));

    sem_wait(&writing_mutex);
    if(i==N)cout<<"Monitor thread: Buffer full!!\n";
    sem_post(&writing_mutex);

    int msgmtxvalue;
    sem_getvalue(&message_mutex,&msgmtxvalue);
    if(msgmtxvalue<=0){
        sem_wait(&writing_mutex);
        cout<<"Monitor thread: waiting to read counter\n";
        sem_post(&writing_mutex);
    }

    sem_wait(&message_mutex);
    sem_wait(&writing_mutex);
    cout<<"Monitor thread: reading a count value of "<<cnt<<"\n";
    sem_post(&writing_mutex);
    int value=cnt;
    cnt=0;
    sem_post(&message_mutex);

    sem_wait(&sem_empty);
    sem_wait(&buffer_mutex);

    sem_wait(&writing_mutex);
    cout<<"Monitor thread: writing to buffer at position "<<i++<<"\n";
    sem_post(&writing_mutex);

    buffer[i]=value;
    sem_post(&buffer_mutex);
    sem_post(&sem_full);

    }
}

void* Collector(void *arg){
    while(1){
    //Sleep(rand()%3);
    //sleep(rand()%3);
    this_thread::sleep_for(chrono::milliseconds(rand()%5));

    sem_wait(&writing_mutex);
    if(!i)cout<<"Collector thread: nothing is in the buffer!\n";
    sem_post(&writing_mutex);

    sem_wait(&sem_full);
    sem_wait(&buffer_mutex);

    sem_wait(&writing_mutex);
    cout<<"Collector thread: reading from buffer at position "<<--i<<"\n";
    sem_post(&writing_mutex);

    sem_post(&buffer_mutex);
    sem_post(&sem_empty);
    }
}

void init(){
    for(int j=0;j<1000;++j)arr[j]=j+1;
    sem_init(&message_mutex, 0, 1);
    sem_init(&buffer_mutex,0,1);
    sem_init(&sem_full,0,0);
    sem_init(&sem_empty,0,N);
    sem_init(&writing_mutex,0,1);
}

int main()
{
  init();
  cin>>n;
  if(n){
    pthread_create(&monitorthread,NULL,&Monitor,NULL);
    pthread_create(&collectorthread,NULL,&Collector,NULL);
    //create messages
    for(int j=0;j<n;j++){
        pthread_create(&p[j],NULL,&Message,&arr[j]);
    }
    for(int j=0;j<n;j++){
        pthread_join(p[j],NULL);
    }
    return 0;
    }
}
