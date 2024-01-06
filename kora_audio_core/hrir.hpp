#pragma once

#include <math.h>
#include <unordered_map>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>


class hrir
{
private:
    std::unordered_map<u_int8_t, std::unordered_map<float, std::vector<float>>> ir_l;
    std::unordered_map<u_int8_t, std::unordered_map<float, std::vector<float>>> ir_r;
    static const size_t azimuths_size = 27;
    const int8_t azimuths[azimuths_size] = { -90, -80, -65, -55, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 55, 65, 80, 90 };
    static const size_t elevations_size = 50;

    std::vector<float> take(std::vector<float> *data, size_t from, size_t to)
    {
        std::vector<float> ret;
        for(auto i(from); i<to; ++i)
        {
            ret.push_back((*data)[i]);
        }

        return ret;
    }

public:

    const std::vector<float> hi_pass_kern =
    {
        0.954939,-0.088056,-0.083983,-0.080026,-0.076184,-0.072455,-0.068837,-0.065328,-0.061925,-0.058627,-0.055432,-0.052337,-0.049341,-0.046443,-0.043639,-0.040928,-0.038309,-0.035779,-0.033337,-0.030980,-0.028708,-0.026518,-0.024408,-0.022377,-0.020424,-0.018545,-0.016740,-0.015008,-0.013346,-0.011752,-0.010226,-0.008765,-0.007369,-0.006034,-0.004761,-0.003548,-0.002392,-0.001293,-0.000249,0.000741,0.001679,0.002567,0.003405,0.004194,0.004937,0.005635,0.006288,0.006898,0.007467,0.007996,0.008485,0.008936,0.009351,0.009730,0.010074,0.010386,0.010665,0.010913,0.011131,0.011320,0.011481,0.011616,0.011724,0.011808,0.011868,0.011904,0.011919,0.011913,0.011886,0.011841,0.011776,0.011695,0.011596,0.011481,0.011352,0.011208,0.011050,0.010880,0.010697,0.010503,0.010298,0.010084,0.009860,0.009627,0.009386,0.009137,0.008882,0.008621,0.008353,0.008081,0.007804,0.007523,0.007238,0.006950,0.006660,0.006367,0.006073,0.005777,0.005481,0.005184,0.004887,0.004591,0.004295,0.004000,0.003706,0.003415,0.003125,0.002838,0.002553,0.002271,0.001992,0.001717,0.001445,0.001178,0.000914,0.000655,0.000400,0.000150,-0.000096,-0.000336,-0.000571,-0.000801,-0.001025,-0.001244,-0.001457,-0.001664,-0.001866,-0.002061,-0.002251,-0.002434,-0.002611,-0.002782,-0.002947,-0.003105,-0.003257,-0.003403,-0.003542,-0.003675,-0.003801,-0.003922,-0.004035,-0.004143,-0.004244,-0.004339,-0.004427,-0.004510,-0.004586,-0.004656,-0.004720,-0.004778,-0.004830,-0.004877,-0.004917,-0.004952,-0.004981,-0.005005,-0.005023,-0.005036,-0.005044,-0.005046,-0.005044,-0.005036,-0.005024,-0.005007,-0.004985,-0.004959,-0.004928,-0.004893,-0.004854,-0.004811,-0.004764,-0.004713,-0.004659,-0.004601,-0.004539,-0.004475,-0.004407,-0.004336,-0.004262,-0.004185,-0.004106,-0.004024,-0.003940,-0.003853,-0.003764,-0.003674,-0.003581,-0.003486,-0.003390,-0.003292,-0.003193,-0.003093,-0.002991,-0.002888,-0.002785,-0.002680,-0.002575,-0.002469,-0.002362,-0.002256,-0.002148,-0.002041,-0.001934,-0.001826,-0.001719,-0.001611,-0.001505,-0.001398,-0.001292,-0.001187,-0.001082,-0.000978,-0.000874,-0.000772,-0.000670,-0.000570,-0.000471,-0.000373,-0.000276,-0.000180,-0.000086,0.000007,0.000098,0.000188,0.000276,0.000363,0.000447,0.000531,0.000612,0.000691,0.000769,0.000845,0.000918,0.000990,0.001060,0.001128,0.001194,0.001258,0.001319,0.001379,0.001436,0.001491,0.001545,0.001596,0.001644,0.001691,0.001736,0.001778,0.001818,0.001856,0.001892,0.001926,0.001957,0.001987,0.002014,0.002039,0.002062,0.002083,0.002102,0.002119,0.002134,0.002147,0.002157,0.002166,0.002173,0.002178,0.002181,0.002183,0.002182,0.002180,0.002176,0.002170,0.002163,0.002154,0.002143,0.002131,0.002117,0.002101,0.002085,0.002066,0.002047,0.002026,0.002003,0.001980,0.001955,0.001929,0.001902,0.001873,0.001844,0.001814,0.001782,0.001750,0.001717,0.001683,0.001648,0.001612,0.001576,0.001539,0.001501,0.001463,0.001424,0.001385,0.001345,0.001305,0.001264,0.001223,0.001182,0.001140,0.001098,0.001056,0.001014,0.000972,0.000930,0.000887,0.000845,0.000802,0.000760,0.000717,0.000675,0.000633,0.000591,0.000550,0.000508,0.000467,0.000426,0.000386,0.000346,0.000306,0.000267,0.000228,0.000190,0.000152,0.000114,0.000077,0.000041,0.000005,-0.000030,-0.000065,-0.000099,-0.000132,-0.000165,-0.000196,-0.000228,-0.000258,-0.000288,-0.000317,-0.000346,-0.000373,-0.000400,-0.000426,-0.000452,-0.000476,-0.000500,-0.000523,-0.000545,-0.000567,-0.000587,-0.000607,-0.000626,-0.000644,-0.000661,-0.000678,-0.000693,-0.000708,-0.000722,-0.000736,-0.000748,-0.000760,-0.000770,-0.000781,-0.000790,-0.000798,-0.000806,-0.000813,-0.000819,-0.000824,-0.000829,-0.000833,-0.000836,-0.000839,-0.000840,-0.000841,-0.000842,-0.000841,-0.000840,-0.000839,-0.000837,-0.000834,-0.000830,-0.000826,-0.000822,-0.000816,-0.000810,-0.000804,-0.000797,-0.000790,-0.000782,-0.000774,-0.000765,-0.000755,-0.000746,-0.000736,-0.000725,-0.000714,-0.000703,-0.000691,-0.000679,-0.000666,-0.000654,-0.000641,-0.000627,-0.000614,-0.000600,-0.000586,-0.000572,-0.000557,-0.000543,-0.000528,-0.000513,-0.000498,-0.000482,-0.000467,-0.000451,-0.000436,-0.000420,-0.000404,-0.000388,-0.000373,-0.000357,-0.000341,-0.000325,-0.000309,-0.000293,-0.000278,-0.000262,-0.000246,-0.000231,-0.000215,-0.000200,-0.000184,-0.000169,-0.000154,-0.000139,-0.000125,-0.000110,-0.000096,-0.000081,-0.000067,-0.000053,-0.000040,-0.000026,-0.000013,0.000000,0.000013,0.000025,0.000038,0.000050,0.000062,0.000073,0.000085,0.000096,0.000106,0.000117,0.000127,0.000137,0.000147,0.000156,0.000165,0.000174,0.000183,0.000191,0.000199,0.000206,0.000214,0.000221,0.000227,0.000234,0.000240,0.000246,0.000251,0.000257,0.000261,0.000266,0.000270,0.000274,0.000278,0.000282,0.000285,0.000288,0.000290,0.000293,0.000295,0.000297,0.000298,0.000299,0.000300,0.000301,0.000301,0.000302,0.000302,0.000301,0.000301,0.000300,0.000299,0.000298,0.000297,0.000295,0.000293,0.000291,0.000289,0.000286,0.000284,0.000281,0.000278,0.000275,0.000272,0.000268,0.000265,0.000261,0.000257,0.000253,0.000249,0.000244,0.000240,0.000235,0.000231,0.000226,0.000221,0.000216,0.000211,0.000206,0.000201,0.000195,0.000190,0.000185,0.000179,0.000174,0.000168,0.000163,0.000157,0.000151,0.000146,0.000140,0.000134,0.000129,0.000123,0.000117,0.000111,0.000106,0.000100,0.000094,0.000089,0.000083,0.000078,0.000072,0.000067,0.000061,0.000056,0.000050,0.000045,0.000040,0.000035,0.000029,0.000024,0.000019,0.000014,0.000010,0.000005,0.000000,-0.000004,-0.000009,-0.000013,-0.000018,-0.000022,-0.000026,-0.000030,-0.000034,-0.000038,-0.000042,-0.000046,-0.000049,-0.000053,-0.000056,-0.000059,-0.000063,-0.000066,-0.000069,-0.000071,-0.000074,-0.000077,-0.000079,-0.000082,-0.000084,-0.000086,-0.000088,-0.000090,-0.000092,-0.000094,-0.000096,-0.000097,-0.000099,-0.000100,-0.000101,-0.000102,-0.000103,-0.000104,-0.000105,-0.000106,-0.000107,-0.000107,-0.000108,-0.000108,-0.000108,-0.000108,-0.000108,-0.000108,-0.000108,-0.000108,-0.000108,-0.000108,-0.000107,-0.000107,-0.000106,-0.000105,-0.000105,-0.000104,-0.000103,-0.000102,-0.000101,-0.000100,-0.000099,-0.000098,-0.000096,-0.000095,-0.000094,-0.000092,-0.000091,-0.000089,-0.000088,-0.000086,-0.000085,-0.000083,-0.000081,-0.000079,-0.000078,-0.000076,-0.000074,-0.000072,-0.000070,-0.000068,-0.000066,-0.000064,-0.000062,-0.000060,-0.000058,-0.000056,-0.000054,-0.000052,-0.000050,-0.000048,-0.000046,-0.000044,-0.000042,-0.000040,-0.000038,-0.000036,-0.000034,-0.000032,-0.000030,-0.000028,-0.000026,-0.000024,-0.000022,-0.000020,-0.000018,-0.000016,-0.000014,-0.000012,-0.000010,-0.000009,-0.000007,-0.000005,-0.000003,-0.000002,0.000000,0.000002,0.000003,0.000005,0.000007,0.000008,0.000010,0.000011,0.000012,0.000014,0.000015,0.000017,0.000018,0.000019,0.000020,0.000021,0.000023,0.000024,0.000025,0.000026,0.000027,0.000028,0.000029,0.000030,0.000030,0.000031,0.000032,0.000033,0.000033,0.000034,0.000035,0.000035,0.000036,0.000036,0.000037,0.000037,0.000037,0.000038,0.000038,0.000038,0.000038,0.000039,0.000039,0.000039,0.000039,0.000039,0.000039,0.000039,0.000039,0.000039,0.000039,0.000039,0.000039,0.000038,0.000038,0.000038,0.000038,0.000037,0.000037,0.000037,0.000036,0.000036,0.000036,0.000035,0.000035,0.000034,0.000034,0.000033,0.000033,0.000032,0.000032,0.000031,0.000030,0.000030,0.000029,0.000029,0.000028,0.000027,0.000027,0.000026,0.000025,0.000025,0.000024,0.000023,0.000022,0.000022,0.000021,0.000020,0.000020,0.000019,0.000018,0.000017,0.000017,0.000016,0.000015,0.000014,0.000014,0.000013,0.000012,0.000011,0.000011,0.000010,0.000009,0.000009,0.000008,0.000007,0.000006,0.000006,0.000005,0.000004,0.000004,0.000003,0.000002,0.000002,0.000001,0.000001,-0.000000,-0.000001,-0.000001,-0.000002,-0.000002,-0.000003,-0.000003,-0.000004,-0.000005,-0.000005,-0.000006,-0.000006,-0.000006,-0.000007,-0.000007,-0.000008,-0.000008,-0.000009,-0.000009,-0.000009,-0.000010,-0.000010,-0.000010,-0.000011,-0.000011,-0.000011,-0.000012,-0.000012,-0.000012,-0.000012,-0.000012,-0.000013,-0.000013,-0.000013,-0.000013,-0.000013,-0.000013,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000014,-0.000013,-0.000013,-0.000013,-0.000013,-0.000013,-0.000013,-0.000013,-0.000012,-0.000012,-0.000012,-0.000012,-0.000012,-0.000012,-0.000011,-0.000011,-0.000011,-0.000011,-0.000010,-0.000010,-0.000010,-0.000010,-0.000010,-0.000009,-0.000009,-0.000009,-0.000009
    };

    const std::vector<float> lo_pass_kern = {0.000000,0.000000,0.000000,0.000000,0.000000,0.000001,0.000001,0.000002,0.000003,0.000005,0.000007,0.000010,0.000013,0.000018,0.000024,0.000030,0.000038,0.000048,0.000059,0.000072,0.000086,0.000103,0.000121,0.000142,0.000165,0.000190,0.000218,0.000248,0.000281,0.000317,0.000356,0.000398,0.000442,0.000490,0.000541,0.000595,0.000652,0.000712,0.000776,0.000843,0.000913,0.000987,0.001064,0.001145,0.001228,0.001315,0.001406,0.001499,0.001596,0.001696,0.001800,0.001906,0.002015,0.002127,0.002242,0.002360,0.002481,0.002604,0.002730,0.002858,0.002989,0.003121,0.003256,0.003393,0.003532,0.003673,0.003815,0.003959,0.004104,0.004250,0.004398,0.004547,0.004696,0.004846,0.004997,0.005148,0.005299,0.005451,0.005603,0.005754,0.005905,0.006056,0.006206,0.006356,0.006505,0.006652,0.006799,0.006944,0.007088,0.007231,0.007371,0.007510,0.007647,0.007782,0.007915,0.008045,0.008173,0.008299,0.008422,0.008542,0.008659,0.008773,0.008884,0.008992,0.009097,0.009198,0.009296,0.009390,0.009481,0.009567,0.009651,0.009730,0.009805,0.009877,0.009944,0.010008,0.010067,0.010122,0.010173,0.010219,0.010262,0.010300,0.010333,0.010363,0.010387,0.010408,0.010424,0.010436,0.010443,0.010446,0.010444,0.010438,0.010428,0.010413,0.010394,0.010371,0.010344,0.010312,0.010276,0.010235,0.010191,0.010142,0.010090,0.010033,0.009973,0.009908,0.009840,0.009768,0.009693,0.009613,0.009530,0.009444,0.009354,0.009261,0.009165,0.009065,0.008962,0.008856,0.008748,0.008636,0.008522,0.008405,0.008285,0.008163,0.008039,0.007912,0.007783,0.007652,0.007519,0.007384,0.007247,0.007109,0.006969,0.006827,0.006684,0.006539,0.006394,0.006247,0.006099,0.005950,0.005801,0.005651,0.005500,0.005348,0.005197,0.005044,0.004892,0.004739,0.004587,0.004434,0.004282,0.004130,0.003978,0.003826,0.003675,0.003525,0.003375,0.003226,0.003078,0.002930,0.002784,0.002639,0.002495,0.002352,0.002210,0.002070,0.001931,0.001794,0.001658,0.001523,0.001391,0.001260,0.001131,0.001004,0.000878,0.000755,0.000634,0.000514,0.000397,0.000282,0.000169,0.000058,-0.000050,-0.000156,-0.000260,-0.000362,-0.000461,-0.000558,-0.000652,-0.000744,-0.000834,-0.000921,-0.001005,-0.001087,-0.001167,-0.001243,-0.001318,-0.001390,-0.001459,-0.001526,-0.001590,-0.001651,-0.001711,-0.001767,-0.001821,-0.001872,-0.001921,-0.001968,-0.002012,-0.002053,-0.002092,-0.002128,-0.002162,-0.002194,-0.002223,-0.002250,-0.002275,-0.002297,-0.002317,-0.002334,-0.002350,-0.002363,-0.002374,-0.002383,-0.002390,-0.002395,-0.002397,-0.002398,-0.002397,-0.002394,-0.002388,-0.002382,-0.002373,-0.002362,-0.002350,-0.002336,-0.002321,-0.002304,-0.002285,-0.002265,-0.002243,-0.002220,-0.002195,-0.002170,-0.002143,-0.002114,-0.002085,-0.002054,-0.002022,-0.001990,-0.001956,-0.001921,-0.001885,-0.001849,-0.001811,-0.001773,-0.001734,-0.001695,-0.001654,-0.001614,-0.001572,-0.001530,-0.001488,-0.001445,-0.001402,-0.001358,-0.001314,-0.001270,-0.001226,-0.001181,-0.001136,-0.001092,-0.001047,-0.001002,-0.000957,-0.000912,-0.000867,-0.000823,-0.000778,-0.000734,-0.000690,-0.000646,-0.000602,-0.000559,-0.000516,-0.000473,-0.000431,-0.000389,-0.000347,-0.000307,-0.000266,-0.000226,-0.000187,-0.000148,-0.000110,-0.000072,-0.000035,0.000001,0.000037,0.000072,0.000106,0.000140,0.000173,0.000205,0.000237,0.000267,0.000297,0.000327,0.000355,0.000382,0.000409,0.000435,0.000460,0.000485,0.000508,0.000531,0.000552,0.000573,0.000594,0.000613,0.000631,0.000649,0.000666,0.000681,0.000697,0.000711,0.000724,0.000737,0.000748,0.000759,0.000769,0.000779,0.000787,0.000795,0.000802,0.000808,0.000813,0.000818,0.000822,0.000825,0.000828,0.000829,0.000830,0.000831,0.000830,0.000829,0.000827,0.000825,0.000822,0.000818,0.000814,0.000810,0.000804,0.000798,0.000792,0.000785,0.000777,0.000769,0.000761,0.000752,0.000742,0.000733,0.000722,0.000712,0.000701,0.000689,0.000677,0.000665,0.000653,0.000640,0.000627,0.000614,0.000600,0.000586,0.000572,0.000558,0.000543,0.000529,0.000514,0.000499,0.000483,0.000468,0.000453,0.000437,0.000422,0.000406,0.000390,0.000375,0.000359,0.000343,0.000327,0.000311,0.000296,0.000280,0.000264,0.000249,0.000233,0.000218,0.000202,0.000187,0.000172,0.000157,0.000142,0.000127,0.000113,0.000098,0.000084,0.000070,0.000056,0.000043,0.000029,0.000016,0.000003,-0.000010,-0.000023,-0.000035,-0.000047,-0.000059,-0.000070,-0.000082,-0.000093,-0.000104,-0.000114,-0.000124,-0.000134,-0.000144,-0.000154,-0.000163,-0.000171,-0.000180,-0.000188,-0.000196,-0.000204,-0.000211,-0.000218,-0.000225,-0.000231,-0.000238,-0.000243,-0.000249,-0.000254,-0.000259,-0.000264,-0.000268,-0.000272,-0.000276,-0.000280,-0.000283,-0.000286,-0.000288,-0.000291,-0.000293,-0.000295,-0.000296,-0.000298,-0.000299,-0.000299,-0.000300,-0.000300,-0.000300,-0.000300,-0.000299,-0.000299,-0.000298,-0.000297,-0.000295,-0.000294,-0.000292,-0.000290,-0.000288,-0.000285,-0.000283,-0.000280,-0.000277,-0.000274,-0.000271,-0.000267,-0.000263,-0.000260,-0.000256,-0.000252,-0.000248,-0.000243,-0.000239,-0.000234,-0.000230,-0.000225,-0.000220,-0.000215,-0.000210,-0.000205,-0.000200,-0.000195,-0.000189,-0.000184,-0.000179,-0.000173,-0.000168,-0.000162,-0.000156,-0.000151,-0.000145,-0.000140,-0.000134,-0.000128,-0.000122,-0.000117,-0.000111,-0.000105,-0.000100,-0.000094,-0.000088,-0.000083,-0.000077,-0.000072,-0.000066,-0.000061,-0.000055,-0.000050,-0.000045,-0.000040,-0.000034,-0.000029,-0.000024,-0.000019,-0.000014,-0.000009,-0.000005,0.000000,0.000005,0.000009,0.000014,0.000018,0.000022,0.000026,0.000030,0.000034,0.000038,0.000042,0.000046,0.000049,0.000053,0.000056,0.000059,0.000063,0.000066,0.000069,0.000071,0.000074,0.000077,0.000079,0.000082,0.000084,0.000086,0.000088,0.000090,0.000092,0.000094,0.000096,0.000097,0.000099,0.000100,0.000101,0.000102,0.000103,0.000104,0.000105,0.000106,0.000107,0.000107,0.000108,0.000108,0.000108,0.000108,0.000108,0.000108,0.000108,0.000108,0.000108,0.000108,0.000107,0.000107,0.000106,0.000105,0.000105,0.000104,0.000103,0.000102,0.000101,0.000100,0.000099,0.000098,0.000096,0.000095,0.000094,0.000092,0.000091,0.000089,0.000088,0.000086,0.000084,0.000083,0.000081,0.000079,0.000078,0.000076,0.000074,0.000072,0.000070,0.000068,0.000066,0.000064,0.000062,0.000060,0.000058,0.000056,0.000054,0.000052,0.000050,0.000048,0.000046,0.000044,0.000042,0.000040,0.000038,0.000036,0.000034,0.000032,0.000030,0.000028,0.000026,0.000024,0.000022,0.000020,0.000018,0.000016,0.000014,0.000012,0.000010,0.000009,0.000007,0.000005,0.000003,0.000002,-0.000000,-0.000002,-0.000003,-0.000005,-0.000007,-0.000008,-0.000010,-0.000011,-0.000012,-0.000014,-0.000015,-0.000017,-0.000018,-0.000019,-0.000020,-0.000021,-0.000023,-0.000024,-0.000025,-0.000026,-0.000027,-0.000028,-0.000029,-0.000030,-0.000030,-0.000031,-0.000032,-0.000033,-0.000033,-0.000034,-0.000035,-0.000035,-0.000036,-0.000036,-0.000037,-0.000037,-0.000037,-0.000038,-0.000038,-0.000038,-0.000038,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000039,-0.000038,-0.000038,-0.000038,-0.000038,-0.000037,-0.000037,-0.000037,-0.000036,-0.000036,-0.000036,-0.000035,-0.000035,-0.000034,-0.000034,-0.000033,-0.000033,-0.000032,-0.000032,-0.000031,-0.000030,-0.000030,-0.000029,-0.000029,-0.000028,-0.000027,-0.000027,-0.000026,-0.000025,-0.000025,-0.000024,-0.000023,-0.000022,-0.000022,-0.000021,-0.000020,-0.000020,-0.000019,-0.000018,-0.000017,-0.000017,-0.000016,-0.000015,-0.000014,-0.000014,-0.000013,-0.000012,-0.000011,-0.000011,-0.000010,-0.000009,-0.000009,-0.000008,-0.000007,-0.000006,-0.000006,-0.000005,-0.000004,-0.000004,-0.000003,-0.000002,-0.000002,-0.000001,-0.000001,0.000000,0.000001,0.000001,0.000002,0.000002,0.000003,0.000003,0.000004,0.000005,0.000005,0.000006,0.000006,0.000006,0.000007,0.000007,0.000008,0.000008,0.000009,0.000009,0.000009,0.000010,0.000010,0.000010,0.000011,0.000011,0.000011,0.000012,0.000012,0.000012,0.000012,0.000012,0.000013,0.000013,0.000013,0.000013,0.000013,0.000013,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000014,0.000013,0.000013,0.000013,0.000013,0.000013,0.000013,0.000013,0.000012,0.000012,0.000012,0.000012,0.000012,0.000012,0.000011,0.000011,0.000011,0.000011,0.000010,0.000010,0.000010,0.000010,0.000010,0.000009,0.000009,0.000009,0.000009};

    const int ir_size = 200;

    hrir()
    {
        ;
    }

    void load(std::string filename)
    {
        std::vector<float> data;

        float f;
        std::ifstream fin(filename, std::ios::binary);
        while (fin.read(reinterpret_cast<char*>(&f), sizeof(float)))
        {
            data.push_back(f);
        }

        int caret = 0;

        for (auto& azm : azimuths)
		{
            std::unordered_map<float, std::vector<float>> el_l;
            std::unordered_map<float, std::vector<float>> el_r;

            auto start_l = caret;
            auto end_l = caret + ir_size;
            auto start_r = caret + ir_size;
            auto end_r = caret + 2*ir_size;

            std::vector<float> el_ldx = take(&data, start_l, end_l);
            std::vector<float> el_rdx = take(&data, start_r, end_r);

            el_l[-90] = el_ldx;
            el_r[-90] = el_rdx;

            caret += 2*ir_size;

            for (int i = 1; i < 51; ++i)
			{
                start_l = caret;
                end_l = caret + ir_size;
                start_r = caret + ir_size;
                end_r = caret + 2*ir_size;

                std::vector<float> el_ld = take(&data, start_l, end_l);
                std::vector<float> el_rd = take(&data, start_r, end_r);

                float elev = -45.f + 5.625f * (i - 1);

                el_l[elev] = el_ld;
                el_r[elev] = el_rd;

                caret += 2*ir_size;
			}

            start_l = caret;
            end_l = caret + ir_size;
            start_r = caret + ir_size;
            end_r = caret + 2*ir_size;

            std::vector<float> el_ldy = take(&data, start_l, end_l);
            std::vector<float> el_rdy = take(&data, start_r, end_r);

            el_l[270] = el_ldy;
            el_r[270] = el_rdy;

            caret += 2*ir_size;

            ir_l[azm] = el_l;
            ir_r[azm] = el_r;
		}

		/*
-90
-45
-39.375
-33.75
-28.125
-22.5
-16.875
-11.25
-5.625
0
5.625
11.25
16.875
22.5
28.125
33.75
39.375
45
50.625
56.25
61.875
67.5
73.125
78.75
84.375
90
95.625
101.25
106.875
112.5
118.125
123.75
129.375
135
140.625
146.25
151.875
157.5
163.125
168.75
174.375
180
185.625
191.25
196.875
202.5
208.125
213.75
219.375
225
230.625
270

{ -90, -80, -65, -55, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 55, 65, 80, 90 }
         */
    }

    std::tuple<std::vector<float>, std::vector<float>> get(int8_t azimuth, float elevation)
    {
        return {ir_l[azimuth][elevation], ir_r[azimuth][elevation]};
    }

    std::tuple<std::vector<float>, std::vector<float>> get_lopass()
    {
        return {lo_pass_kern, lo_pass_kern};
    }

    std::tuple<std::vector<float>, std::vector<float>> get_hipass()
    {
        return {hi_pass_kern, hi_pass_kern};
    }

    /*
    void loadHrir(std::string filename)
    {
        std::FileInputStream istream(filename);
        if (istream.openedOk())
        {
            std::vector<Vec2f> points;
            int azimuths[] = {-90, -80, -65, -55, -45, -40, -35, -30, -25, -20,
                -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 55, 65, 80, 90};
            for (auto& azm : azimuths)
            {
                hrirDict_.insert(std::make_pair(azm, std::array<HRIRBuffer, 52>()));
                // -90 deg
                istream.read(hrirDict_[azm][0].leftEarIR.data(), 200 * sizeof(float));
                istream.read(hrirDict_[azm][0].rightEarIR.data(), 200 * sizeof(float));
                points.push_back({static_cast<float>(azm), -90.f});
                // 50 elevations
                for (int i = 1; i < 51; ++i)
                {
                    istream.read(hrirDict_[azm][i].leftEarIR.data(), 200 * sizeof(float));
                    istream.read(hrirDict_[azm][i].rightEarIR.data(), 200 * sizeof(float));
                    points.push_back({static_cast<float>(azm), -45.f + 5.625f * (i - 1)});
                }
                // 270 deg
                istream.read(hrirDict_[azm][51].leftEarIR.data(), 200 * sizeof(float));
                istream.read(hrirDict_[azm][51].rightEarIR.data(), 200 * sizeof(float));
                points.push_back({static_cast<float>(azm), 270});
            }
            triangulation_ = new Delaunay();
            triangulation_->triangulate(points);
        }
        else
            throw std::ios_base::failure("Failed to open HRIR file");
    }*/
};