#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>

using namespace std;
const int N=100;
const int M=-5000;
int n,people[N][2];//0=force,1=ability
int save[N][N*100+1],s[N];
int ans=-65535;

int solve(int cnt,int Fsum,int Asum){
   if(cnt==n){
        if(Fsum<=0)return M-1;
        if(Asum>ans)ans = Asum;
        return Asum;
   }
   if(save[cnt][Fsum+5000]!=M-1){//如果被修改過
       Asum+=save[cnt][Fsum+5000];
       if(Asum>ans)ans=Asum;
       return Asum;
   }
   for(s[cnt]=0;s[cnt]<=1;s[cnt]++){//是否用當前的人
       if(s[cnt])Fsum+=people[cnt][0];
       if(s[cnt])Asum+=people[cnt][1];
       
       int temp=solve(cnt+1,Fsum,Asum);//直到下一步的決策
       if(s[cnt])Fsum-=people[cnt][0];
       if(s[cnt])Asum-=people[cnt][1];
       temp-=Asum;//記錄當前人之前的工作能力
       if(temp > save[cnt][Fsum+5000]){
           save[cnt][Fsum+5000] = temp;
       }
   }
   return Asum+save[cnt][Fsum+5000];
}

int main(){ 
    cin>>n;
    for(int i=0;i<n;i++){
        cin>>people[i][0];
    }
    for(int i=0;i<n;i++){
        cin>>people[i][1];
    }
    for(int i=0;i<N;i++){
        for(int j=0;j<N*100+1;j++){
            save[i][j]=M-1;//Fsum在第i步的最大值
        }
    }
    solve(0,0,0);
    cout<<ans<<endl;

    return 0;
}