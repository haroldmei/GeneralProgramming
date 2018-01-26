#include <stdio.h>
#include <math.h>
#include <string.h>

#define PT_SINGLE 1	             /* One card, 1*/
#define PT_PAIR 2                /* Two cards as a pair, 2*/
#define PT_TRIPLE 3              /* A triple of cards, 3*/
#define PT_TRIPLE_PLUS_SINGLE 4  /* A triple of cards plus a different one, 4*/
#define PT_TRIPLE_PLUS_PAIR 5    /* A triple of cards plus a diffenent pair, 5*/
#define PT_FOUR_PLUS_SINGLES 6   /* Four cards in the same ranks plus two singles, 5*/
#define PT_FOUR_PLUS_PAIRS 7     /* Four cards in the same ranks plus two pairs, 6*/
#define PT_SINGLE_SERIAL 8       /* A straight of n consecutive single cards, n*1 */
#define PT_PAIR_SERIAL 9         /* A straight of n consecutive pairs, n*2 */
#define PT_TRIPLE_SERIAL 10       /* A straight of n consecutive triples, n*3 */
#define PT_TRIPLE_SERIAL_PLUS_SINGLES 11  /* A triple straight of n plus either n singles, n*(3+1) */
#define PT_TRIPLE_SERIAL_PLUS_PAIRS 12    /* A triple straight of n plus either n triples, n*(3+2) */
#define PT_BOMB 13               /* Four cards in same rank,4*/
#define PT_NUKE 14               /* Wwo jokers (CB), 2*/

#define BIT(A) (1 << (A))
#define NUKEVALUE 0x6000         /* BIT14: color ghost; BIT13: black ghost*/
#define GETVALUE(A) BIT((A) - '3')

char g_szKeys[] = "3456789XJQKA2BC";
struct stPattern
{
    int nOne;
    int nTwo;
    int nThree;
    int nFour;
};


bool isCont(int n, int cnt)
{
    int nTmp = (1 << cnt) - 1;
    for (int i = 0; i < (16 - cnt); i++)
    {
        if ((n & (nTmp << i)) == n)
        {
            return true;
        }
    }
    return false;
}

int getCnt(int n)
{
    int nCnt = 0; 
    for (int i = 0; i < 16; i++)
    {
        if (n & (1 << i))
        {
            nCnt++;
        }
    }
    return nCnt;
}


bool chkstrAndReplace(char* str)
{
    int i = 0;
    int nLen = strlen(str);
    for (; i < nLen; i++)
    {
        switch (str[i])
        {
        case 'X':
            str[i] = '9' + 1;
            break;
        case 'J':
            str[i] = '9' + 2;
            break;
        case 'Q':
            str[i] = '9' + 3;
            break;
        case 'K':
            str[i] = '9' + 4;
            break;
        case 'A':
            str[i] = '9' + 5;
            break;
        case '2':
            str[i] = '9' + 6;
            break;
        case 'B':
            str[i] = '9' + 7;
            break;
        case 'C':
            str[i] = '9' + 8;
            break;
        default:
            if ('3' > str[i] || str[i] > '9')
            {
                return false;
            }
            break;
        }
    }

    return true;
}


void getPattern(char* str, stPattern* pattern)
{
    int nLen = strlen(str);

    int nOne = 0;
    int nTwo = 0;
    int nThree = 0;
    int nFour = 0;

    for (int i = 0; i < nLen; i++)
    {
        if (!(nOne & GETVALUE(str[i])))
        {
            nOne |= GETVALUE(str[i]);
        }
        else
        {
            if (!(nTwo & GETVALUE(str[i])))
            {
                nTwo |= GETVALUE(str[i]);
            }
            else
            {
                if (!(nThree & GETVALUE(str[i])))
                {
                    nThree |= GETVALUE(str[i]);
                }
                else
                {
                    if (!(nFour & GETVALUE(str[i])))
                    {
                        nFour |= GETVALUE(str[i]);
                    }
                }
            }
        }
    }
    
    pattern->nOne = nOne - (nTwo | nThree | nFour);
    pattern->nTwo = nTwo - (nThree | nFour);
    pattern->nThree = nThree - nFour;
    pattern->nFour = nFour;
    return;
}

int checkPattern(char* str)
{
    stPattern pattern = {0};
    int nOne = 0;
    int nTwo = 0;
    int nThree = 0;
    int nFour = 0;
    int nOneC = 0;
    int nTwoC = 0;
    int nThreeC = 0;
    int nFourC = 0;
    int nLen = strlen(str);
    
    chkstrAndReplace(str);
    getPattern(str, &pattern);
    nOne = pattern.nOne;
    nTwo = pattern.nTwo;
    nThree = pattern.nThree;
    nFour = pattern.nFour;
    
    if ((nOne & NUKEVALUE) && (0 == nOne - NUKEVALUE) && !nTwo && !nThree && !nFour) return PT_NUKE << 16;
    if (nOne && !(nOne & (nOne-1)) && !nTwo && !nThree && !nFour) return nOne | (PT_SINGLE << 16);
    if (nTwo && !nOne && !(nTwo & (nTwo - 1)) && !nThree && !nFour) return nTwo | (PT_PAIR << 16);
    if (nThree && !nOne && !nTwo && !(nThree & (nThree - 1)) && !nFour) return nThree | (PT_TRIPLE << 16);
    if (nFour && !nOne && !nTwo && !nThree && !(nFour & (nFour - 1))) return nFour | (PT_BOMB << 16);
    if (nOne && !(nOne & (nOne - 1)) && !nTwo && nThree && !(nThree & (nThree - 1)) && !nFour) return nThree | (PT_TRIPLE_PLUS_SINGLE << 16);
    if (nTwo && !(nTwo & (nTwo - 1)) && !nOne && nThree && !(nThree & (nThree - 1)) && !nFour) return nThree | (PT_TRIPLE_PLUS_PAIR << 16);

    nOneC = getCnt(nOne);
    if (nOne && (nOneC == 2) && !nTwo && !nThree && nFour && !(nFour & (nFour - 1))) return nFour | (PT_FOUR_PLUS_SINGLES << 16);

    nTwoC = getCnt(nTwo);
    if (nTwo && (nTwoC == 2) && !nOne && !nThree && nFour && !(nFour & (nFour - 1))) return nFour | (PT_FOUR_PLUS_PAIRS << 16);
    if (nOne && (nOneC > 4) && isCont(nOne, nOneC) && !nTwo && !nThree && !nFour) return nOne | (PT_SINGLE_SERIAL << 16);
    if (nTwo && (nTwoC > 2) && isCont(nTwo, nTwoC) && !nOne && !nThree && !nFour) return nTwo | (PT_PAIR_SERIAL << 16);

    nThreeC = getCnt(nThree);
    if (nThree && (nThreeC > 1) && isCont(nThree, nThreeC) && !nTwo && !nOne && !nFour) return nThree | (PT_TRIPLE_SERIAL << 16);
    
    if (nThree && (nThreeC > 1) && isCont(nThree, nThreeC) && !nTwo && (nOneC == nThreeC) && !nFour) return nThree | (PT_TRIPLE_SERIAL_PLUS_SINGLES << 16);
    if (nThree && (nThreeC > 1) && isCont(nThree, nThreeC) && (nTwoC == nThreeC) && !nOne && !nFour) return nThree | (PT_TRIPLE_SERIAL_PLUS_PAIRS << 16);

    return 0;
}

bool play(char* ptn, char* cards, int nPattern, stPattern *outPattern)
{
    stPattern pattern = {0};
    bool ret = false;
    int nOne = 0;
    int nTwo = 0;
    int nThree = 0;
    int nFour = 0;
    int nMagic = 0;
    int nKey = 0;

    chkstrAndReplace(cards);
    getPattern(cards, &pattern);
    nOne = pattern.nOne;
    nTwo = pattern.nTwo;
    nThree = pattern.nThree;
    nFour = pattern.nFour;
    
    nKey = nPattern & 0xffff;
    if ((nOne & NUKEVALUE) == NUKEVALUE)
    {
        outPattern->nOne = NUKEVALUE;
        return true;
    }
    
    if (nFour && (nPattern >> 16 < PT_BOMB))
    {
        outPattern->nFour = 1;
        while(!(nFour & outPattern->nFour))
        {
            outPattern->nFour = outPattern->nFour << 1;
        }
        return true;
    }
    
    switch (nPattern >> 16)
    {
    case PT_NUKE:
        break;
        
    case PT_SINGLE: 
        nMagic = nOne | nTwo | nThree | nFour;
        if (nMagic >= nKey * 2) 
        {
            ret = true;
            outPattern->nOne = nKey * 2;
            while(!(nMagic & outPattern->nOne))
            {
                outPattern->nOne = outPattern->nOne << 1;
            }
        }
        break;
        
    case PT_PAIR: 
        nMagic = nTwo | nThree | nFour;
        if (nMagic >= nKey * 2) 
        {
            ret = true;
            outPattern->nTwo = nKey * 2;
            while(!(nMagic & outPattern->nTwo))
            {
                outPattern->nTwo = outPattern->nTwo << 1;
            }
        }
        break;
        
    case PT_TRIPLE: 
        nMagic = nThree | nFour;
        if (nMagic >= nKey * 2)
        {
            ret = true;
            outPattern->nThree = nKey * 2;
            while(!(nMagic & outPattern->nThree))
            {
                outPattern->nThree = outPattern->nThree << 1;
            }
        }
        break;
        
    case PT_BOMB: 
        nMagic = nFour;
        if (nMagic >= nKey * 2) 
        {
            ret = true;
            outPattern->nFour = nKey * 2;
            while(!(nMagic & outPattern->nFour))
            {
                outPattern->nFour = outPattern->nFour << 1;
            }
        }
        break;
        
    case PT_TRIPLE_PLUS_SINGLE: 
        nMagic = nThree | nFour;
        if (nMagic >= nKey * 2 && (getCnt(nMagic | nOne | nTwo) - 1 > 0))
        {
            ret = true;
            
            outPattern->nThree = nKey * 2;
            while(!(nMagic & outPattern->nThree))
            {
                outPattern->nThree = outPattern->nThree << 1;
            }
            
            nMagic = nMagic & ~(outPattern->nThree); /* exclude the selected element */
            nMagic |= (nTwo | nOne);
            outPattern->nOne = 1;
            while(!(nMagic & outPattern->nOne))
            {
                outPattern->nOne = outPattern->nOne << 1;
            }
        }
        break;
        
    case PT_TRIPLE_PLUS_PAIR: 
        nMagic = nThree | nFour;
        if (nMagic >= nKey * 2 && (getCnt(nMagic | nTwo) - 1 > 0))
        {
            ret = true;
            
            outPattern->nThree = nKey * 2;
            while(!(nMagic & outPattern->nThree))
            {
                outPattern->nThree = outPattern->nThree << 1;
            }
            
            nMagic = nMagic & ~(outPattern->nThree); /* exclude the selected element */
            nMagic |= nTwo;
            outPattern->nTwo = 1;
            while(!(nMagic & outPattern->nTwo))
            {
                outPattern->nTwo = outPattern->nTwo << 1;
            }
        }
        break;
        
    case PT_FOUR_PLUS_SINGLES: 
        nMagic = nFour;
        if (nMagic >= nKey * 2 && (getCnt(nMagic | nThree | nTwo | nOne) - 1 > 1))
        {
            int nTmp = 0;
            ret = true;
            
            outPattern->nFour = nKey * 2;
            while(!(nMagic & outPattern->nFour))
            {
                outPattern->nFour = outPattern->nFour << 1;
            }
            
            nMagic = nMagic & ~(outPattern->nFour); /* exclude the selected element */
            nMagic = nMagic | nThree | nTwo | nOne; 
            outPattern->nOne = 1;
            while(!(nMagic & outPattern->nOne))
            {
                outPattern->nOne = outPattern->nOne << 1;
            }

            nMagic = nMagic & ~(outPattern->nOne); /* exclude the selected element */
            nTmp = 1;
            while(!(nMagic & nTmp))
            {
                nTmp = nTmp << 1;
            }
            outPattern->nOne |= nTmp;
        }
        break;
        
    case PT_FOUR_PLUS_PAIRS: /*If the cards contains 4 same key, this case will be ignored. */
        nMagic = nFour;
        if (nMagic >= nKey * 2 && (getCnt(nMagic | nThree | nTwo) - 1 > 1))
        {
            int nTmp = 0;
            ret = true;
            
            outPattern->nFour = nKey * 2;
            while(!(nMagic & outPattern->nFour))
            {
                outPattern->nFour = outPattern->nFour << 1;
            }
            
            nMagic = nMagic & ~(outPattern->nFour); /* exclude the selected element */
            nMagic = nMagic | nThree | nTwo; 
            outPattern->nTwo = 1;
            while(!(nMagic & outPattern->nTwo))
            {
                outPattern->nTwo = outPattern->nTwo << 1;
            }

            nMagic = nMagic & ~(outPattern->nTwo); /* exclude the selected element */
            nTmp = 1;
            while(!(nMagic & nTmp))
            {
                nTmp = nTmp << 1;
            }
            outPattern->nTwo |= nTmp;
        }
        break;
        
    case PT_SINGLE_SERIAL: 
        nMagic = nOne | nTwo | nThree | nFour;
        if (nMagic >= nKey * 2)
        {
            do
            {
                nKey *= 2;
                if ((nKey & nMagic) == nKey)
                {
                    ret = true;
                    outPattern->nOne = nKey;
                    break;
                }
            }while(nMagic >= nKey);
        }
        break;
        
    case PT_PAIR_SERIAL: 
        nMagic = nTwo | nThree | nFour;
        if (nMagic >= nKey * 2)
        {
            do
            {
                nKey *= 2;
                if ((nMagic & nKey) == nKey)
                {
                    ret = true;
                    outPattern->nTwo = nKey;
                    break;
                }
            }while(nMagic >= nKey);
        }
        break;
        
    case PT_TRIPLE_SERIAL: 
        nMagic = nThree | nFour;
        if (nMagic >= nKey * 2)
        {
            do
            {
                nKey *= 2;
                if ((nMagic & nKey) == nKey)
                {
                    ret = true;
                    outPattern->nThree = nKey;
                    break;
                }
            }while(nMagic >= nKey);
        }
        break;
        
    case PT_TRIPLE_SERIAL_PLUS_SINGLES:
        nMagic = nThree | nFour;
        if ((nMagic >= nKey * 2) && (getCnt(nMagic | nOne | nTwo) - 2 * getCnt(nMagic) >= 0))
        {
            int keycnt = getCnt(nKey);
            do
            {
                nKey *= 2;
                if ((nMagic & nKey) == nKey)
                {
                    int tmp = 0;
                    ret = true;
                    outPattern->nThree = nKey;
                    nMagic |= (nOne | nTwo);
                    nMagic &= ~(outPattern->nThree);/* exclude the selected element */
                    for (; nMagic > 0; nMagic /= 2)
                    {
                        tmp++;
                        if ((nMagic & 1) && keycnt)
                        {
                            keycnt--;
                            outPattern->nOne |= (1 << tmp);
                        }
                    }
                    break;
                }
            }while(nMagic >= nKey);
        } 
        break;
        
    case PT_TRIPLE_SERIAL_PLUS_PAIRS: 
        nMagic = nThree | nFour;
        if ((nMagic >= nKey * 2) && (getCnt(nMagic | nTwo) - 2 * getCnt(nMagic) >= 0))
        {
            int keycnt = getCnt(nKey);
            do
            {
                nKey *= 2;
                if ((nMagic & nKey) == nKey)
                {
                    int tmp = 0;
                    ret = true;
                    outPattern->nThree = nKey;
                    nMagic |= (nOne | nTwo);
                    nMagic &= ~(outPattern->nThree);/* exclude the selected element */
                    for (; nMagic > 0; nMagic /= 2)
                    {
                        tmp++;
                        if ((nMagic & 1) && keycnt)
                        {
                            keycnt--;
                            outPattern->nTwo |= (1 << tmp);
                        }
                    }
                    break;
                }
            }while(nMagic >= nKey);
        } 
        break;
        
    default:
        break;
    }

    return ret;
}

void genResult(stPattern* pattern, char* resultStr)
{
    int i = 0;
	char szTmp[8] = {0};
    if (!(pattern->nOne || pattern->nTwo || pattern->nThree || pattern->nFour))
    {
        strcpy(resultStr, "Pass\n");
        return;
    }

    for (; i < 15; i++) /* restore cards */
    {
		szTmp[0] = szTmp[1] = szTmp[2] = szTmp[3] = 0;
        if (pattern->nOne & (1 << i))
        {
			szTmp[0] = g_szKeys[i];
            resultStr = strcat(resultStr, szTmp);
        }
        
        if (pattern->nTwo & (1 << i))
        {
			szTmp[0] = szTmp[1] = g_szKeys[i];
            resultStr = strcat(resultStr, szTmp);
        }
        
        if (pattern->nThree & (1 << i))
        {
			szTmp[0] = szTmp[1] = szTmp[2] = g_szKeys[i];
            resultStr = strcat(resultStr, szTmp);
        }
        
        if (pattern->nFour & (1 << i))
        {
			szTmp[0] = szTmp[1] = szTmp[2] = szTmp[3] = g_szKeys[i];
            resultStr = strcat(resultStr, szTmp);
        }
    }
    
    resultStr = strcat(resultStr, "\n");
    return;
}

int main(int argc, char** argv)
{
    stPattern pattern = {0};
    int nRound = 0;
    char szPattern[56] = {0};
    char szCards[56] = {0};
    char resultStr[56] = {0};
    
    FILE *fp = 0;
    FILE *fpOut = 0;
    int i = 0;
    int nPattern = 0;
    
    if (argc != 2)
    {
        printf("please input 'test.exe inputfile' in console window. press to quit.");
        if (getchar())
        {
            return 0;
        }
    }
    

    if((fp = fopen(argv[1]/*"input.txt"*/, "r")) == NULL)
    {
        printf("input file not open");
		return -1;
    }

    if((fpOut = fopen("output.txt","w")) == NULL)
    {
        printf("cannot open output file");
		return -1;
    }
    
    fscanf(fp, "%d\n", &nRound);
    for (i = 0; (i < nRound) && (feof(fp)==0); i++)
    {
		memset(&pattern, 0, sizeof(pattern));
		memset(resultStr, 0, strlen(resultStr));
        memset(szPattern, 0, strlen(szPattern));
        memset(szCards, 0, strlen(szCards));
        nPattern = 0;
        fscanf(fp, "%s\n", szPattern);
        fscanf(fp, "%s\n", szCards);
		szPattern[strlen(szPattern) - 1] = 0;/*delete #*/
		szCards[strlen(szCards) - 1] = 0;
        
        chkstrAndReplace(szPattern);
        nPattern = checkPattern(szPattern);
        if (!nPattern)
        {
            sprintf(resultStr, "%s is invalid.\n", szPattern);
        }
        else
        {
            play(szPattern, szCards, nPattern, &pattern);
            genResult(&pattern, resultStr);
        }
        fprintf(fpOut, "%s", resultStr);
    }
    fclose(fpOut);
    fclose(fp);


    return 0;
}
