#include <bit>
#include "generalData.h"
#include "magic.h"


ULL raytraceRook(int index, ULL blockers)
{
    ULL legalMoves = 0;
    ULL notBlockers = ~blockers;
    ULL spawn = indexToULL.at(index);
    for (int i = 1; i < 8; i++) // north
    {
        if (((spawn << (8 * i)) & notBlockers) != 0)
        {
            legalMoves ^= spawn << (8 * i);
        }
        else
        {
            legalMoves ^= spawn << (8 * i);
            break;
        }
    }
    for (int i = 1; i < 8; i++) // south
    {
        if (((spawn >> (8 * i)) & notBlockers) != 0)
        {
            legalMoves ^= spawn >> (8 * i);
        }
        else
        {
            legalMoves ^= spawn >> (8 * i);
            break;
        }
    }
    for (int i = 1; i < 8; i++) // east
    {
        if (index % 8 + i < 8 && ((spawn << i) & notBlockers) != 0)
        {
            legalMoves ^= spawn << i;
        }
        else if (index % 8 + i < 8)
        {
            legalMoves ^= spawn << i;
            break;
        }
        else
            break;
    }
    for (int i = 1; i < 8; i++) // west
    {
        if (index % 8 - i >= 0 && ((spawn >> i) & notBlockers) != 0)
        {
            legalMoves ^= spawn >> i;
        }
        else if (index % 8 - i >= 0)
        {
            legalMoves ^= spawn >> i;
            break;
        }
        else
            break;
    }
    return legalMoves;
}

ULL raytraceBishop(int index, ULL blockers)
{
    ULL legalMoves = 0;
    ULL notBlockers = ~blockers;
    ULL spawn = indexToULL.at(index);
    for (int i = 1; i < 8; i++) // northeast
    {
        if (index % 8 + i < 8 && ((spawn << (9 * i)) & notBlockers) != 0)
        {
            legalMoves ^= spawn << (9 * i);
        }
        else if (index % 8 + i < 8)
        {
            legalMoves ^= spawn << (9 * i);
            break;
        }
        else break;
    }
    for (int i = 1; i < 8; i++) // northwest
    {
        if (index % 8 - i >= 0 && ((spawn << (7 * i)) & notBlockers) != 0)
        {
            legalMoves ^= spawn << (7 * i);
        }
        else if (index % 8 - i >= 0)
        {
            legalMoves ^= spawn << (7 * i);
            break;
        }
        else break;
    }
    for (int i = 1; i < 8; i++) // southwest
    {
        if (index % 8 - i >= 0 && ((spawn >> (9 * i)) & notBlockers) != 0)
        {
            legalMoves ^= spawn >> (9 * i);
        }
        else if (index % 8 - i >= 0)
        {
            legalMoves ^= spawn >> (9 * i);
            break;
        }
        else
            break;
    }
    for (int i = 1; i < 8; i++) // southeast
    {
        if (index % 8 + i < 8 && ((spawn >> (7 * i)) & notBlockers) != 0)
        {
            legalMoves ^= spawn >> (7 * i);
        }
        else if (index % 8 + i < 8)
        {
            legalMoves ^= spawn >> (7 * i);
            break;
        }
        else
            break;
    }
    return legalMoves;
}

vector<ULL> allPossibleBlockers(ULL& relevantBits, int& nBits)
{
    vector<ULL> allBlockers;
    ULL blockers = 0;
    for (int i = 0; i < indexToULL.at(64 - nBits); i++)
    {
        blockers ^= ~relevantBits;
        blockers++;
        blockers &= relevantBits;
        allBlockers.push_back(blockers);
        //cout << endl << i << endl;
        //visualizeBoardOld(blockers);
    }
    return allBlockers;
}

void initializeMagicBitboards()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            rook[i * 8 + j].blockerMask = fileMasks[j] & ~rank1Mask & ~rank8Mask & ~indexToULL.at(i * 8 + j);
            rook[i * 8 + j].blockerMask ^= rankMasks[i] & ~fileAMask & ~fileHMask & ~indexToULL.at(i * 8 + j);
            rook[i * 8 + j].nBits = 64 - popcount(rook[i * 8 + j].blockerMask);
        }
    }
    ULL inner6bits = ~(rank1Mask | rank8Mask | fileAMask | fileHMask);
    for (int i = 0; i < 64; i++)
    {
        for (int j = 1; j < 7; j++)
        {
            if (i % 8 + j < 7)
                bishop[i].blockerMask ^= indexToULL.at(i) << (9 * j);
            if (i % 8 - j > 0)
                bishop[i].blockerMask ^= indexToULL.at(i) << (7 * j);
            if (i % 8 - j > 0)
                bishop[i].blockerMask ^= indexToULL.at(i) >> (9 * j);
            if (i % 8 + j < 7)
                bishop[i].blockerMask ^= indexToULL.at(i) >> (7 * j);
        }
        bishop[i].blockerMask &= inner6bits;
        bishop[i].nBits = 64 - popcount(bishop[i].blockerMask);
        //cout << rook[i].blockerMask << ", ";
        //if (i % 8 == 7) cout << endl;
    }
    ULL rookMagicNumbers[64]{
    3638909049044468768,
    18014691909476352,
    2341900187385012225,
    9835867702218983424,
    2449971530080321584,
    2161745449898279424,
    720584738688008704,
    36029896952430720,
    81205532931244032,
    162692680587952385,
    281750207029248,
    3096263484510208,
    36591885048877192,
    563053100794368,
    13980299435456958470,
    140827682690176,
    2317243295679397888,
    10966417925837357088,
    1155473472170754304,
    302736233610612736,
    4614650704494672,
    441635339178868746,
    4398717997057,
    648590914119860292,
    577797760591339648,
    2399276109924608,
    108191946325885313,
    4647997531669659672,
    1205066895982720,
    563645739172872,
    72059810243252232,
    3459894854725304385,
    431283444384800,
    9223812666148003843,
    1441724521265504384,
    436850336397797408,
    2658267306617996544,
    2452774115723970572,
    4611844383736595202,
    4644385509605632,
    148658989670957056,
    9009741948796935,
    4503771493244928,
    5225584385743061025,
    5498766163984,
    72620562379309136,
    5208414240406503440,
    2252917588492292,
    282043156398592,
    1459172366379649152,
    6413161058327937280,
    6773695452183265536,
    140823392419968,
    153203753340895360,
    77124177995502080,
    9448662523441317376,
    1152993939515604997,
    36648964341765,
    42977849053315586,
    4616189686909304865,
    72339078755385361,
    13839843140759191585,
    2341915935328174596,
    27034863316041802
    };
    ULL bishopMagicNumbers[64]{
    1155208523156570240,
    7070656972937037848,
    578818659586293778,
    4790580752158736,
    22817340194299904,
    36595273679704578,
    579312890905886720,
    144680341414675460,
    3458784322900197897,
    145285072877135936,
    576506056172240896,
    4648845177857770049,
    9223376452618027010,
    146367572546723840,
    2306996399093399714,
    99080292538785856,
    18014691374928192,
    4503682037121793,
    1157495481576849920,
    585547133592019076,
    10134207272845448,
    9799973539537748480,
    55177934511670150,
    333301558039171072,
    13517405756081160,
    1130581962327040,
    227451572945289728,
    281754216710656,
    290288266395649,
    866943204522524816,
    5073155043430400,
    591801276006728740,
    1130572833374849,
    2256219340275968,
    4694299488914768896,
    1152992182597878272,
    1127033849774144,
    1157427311981662208,
    9800266022513575936,
    4683886554380372752,
    9512167572217282568,
    9368050744035663880,
    146437632056889346,
    283476494338,
    35193105175552,
    216542222349273154,
    9252654389728928773,
    7287142076213367040,
    2306970455445340168,
    144684739469639682,
    2883434199661938688,
    70370959032580,
    2452351851555987457,
    53945058010176,
    4612987857445355792,
    76715267631097897,
    696088201588261440,
    72235723530504195,
    23644456460886528,
    70953535144448,
    1441750582155027464,
    2305847426722824452,
    18020033976599186,
    4656724215888871552
    };
    for (int i = 0; i < 64; i++)
    {
        rook[i].magic = rookMagicNumbers[i];
        bishop[i].magic = bishopMagicNumbers[i];
    }

    for (int m = 0; m < 128; m++)
    {
        int index = m % 64;
        int pieceType;
        if (m / 64 == 0)
            pieceType = ROOK;
        else
            pieceType = BISHOP;
        ULL relevantBits = 0;
        ULL blockers[4096]{};
        int nBits;
        if (pieceType == ROOK)
        {
            relevantBits = rook[index].blockerMask;
            nBits = rook[index].nBits;
        }
        else
        {
            relevantBits = bishop[index].blockerMask;
            nBits = bishop[index].nBits;
        }
        vector<ULL> blockerVector = allPossibleBlockers(relevantBits, nBits);
        int i = 0;
        for (ULL& blocker : blockerVector)
        {
            blockers[i] = blocker;
            i++;
        }

        ULL magicCandidate;
        if (pieceType == ROOK)
            magicCandidate = rook[index].magic;
        else
            magicCandidate = bishop[index].magic;
        for (int i = 0; i < indexToULL.at(64 - nBits); i++)
        {
            int magicIndex = int((blockers[i] * magicCandidate) >> (nBits));
            if (pieceType == ROOK)
            {
                legalRookMoves[index][magicIndex] = raytraceRook(index, blockers[i]);
            }
            else
            {
                legalBishopMoves[index][magicIndex] = raytraceBishop(index, blockers[i]);
            }
        }
    }
}

ULL getRookMoves(int index, ULL blockers)
{
    blockers &= rook[index].blockerMask;
    blockers = (blockers * rook[index].magic) >> (rook[index].nBits);
    return legalRookMoves[index][blockers];
}

ULL getBishopMoves(int index, ULL blockers)
{
    blockers &= bishop[index].blockerMask;
    blockers = (blockers * bishop[index].magic) >> (bishop[index].nBits);
    return legalBishopMoves[index][blockers];
}