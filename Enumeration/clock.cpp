#include <iostream>
#include <cstring>
using namespace std;

int move[9][5] = {1,2,4,5,0, 1,2,3,0,0, 2,3,5,6,0, 1,4,7,0,0, 2,4,5,6,8, 3,6,9,0,0, 4,5,7,8,0, 7,8,9,0,0, 5,6,8,9,0};

int main()
{
    int time[3][3];
    int i[10] = {0};
    int res[10] = {0};
    int cnt;
    int max = 999999999;

    for( int i = 0; i < 3; i ++)
        for( int j = 0; j < 3; j++)
            cin >> time[i][j];


    for(i[0] = 0; i[0] < 4; i[0]++)
		for(i[1] = 0; i[1] < 4; i[1]++)
			for(i[2] = 0; i[2] < 4; i[2]++)
				for(i[3] = 0; i[3] < 4; i[3]++)
					for(i[4] = 0; i[4] < 4; i[4]++)
						for(i[5] = 0; i[5] < 4; i[5]++)
							for(i[6] = 0; i[6] < 4; i[6]++)
								for(i[7] = 0; i[7] < 4; i[7]++)
									for(i[8] = 0; i[8] < 4; i[8]++)
                                    {
										int ans = 0;
										ans += (time[0][0] + i[0] + i[1] + i[3])%4; 

										ans += (time[0][1] + i[0] + i[1] + i[2] + i[4])%4; 
										ans += (time[0][2] + i[1] + i[2] + i[5])%4; 
										ans += (time[1][0] + i[0] + i[3] + i[4] + i[6])%4; 
										ans += (time[1][1] + i[0] + i[2] + i[4] + i[6] + i[8])%4; 
										ans += (time[1][2] + i[2] + i[4] + i[5] + i[8])%4; 
										ans += (time[2][0] + i[3] + i[6] + i[7])%4; 
										ans += (time[2][1] + i[4] + i[6] + i[7] + i[8])%4; 
										ans += (time[2][2] + i[5] + i[7] + i[8])%4;

										if(ans == 0)
                                        {
											cnt = 0;
											for(int j = 0; j < 9; j++)
                                            {
												cnt += i[j];
											}

											if(cnt < max)
                                            {
												max = cnt;
                                                for( int k = 0; k < 9; k++)
                                                {
                                                    res[k] = i[k];
                                                }
											}
                                        }
                                    }

	for(int i = 0; i < 9; i++)
		for( ; res[i] > 0; res[i]--)
        {
			cout << i + 1 << " ";
		}

    system("pause");

}
