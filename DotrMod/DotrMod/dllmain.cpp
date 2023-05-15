#include "mem.h"
#include "Hook.h"
#include <cstdio>

/*TODO
    Implement string compression algorithm and reduce string space size
    Effect editing stuff (relies on string editing being achievable and completed)
    Maybe able to view last opponents Deck (need to find a spare button on deck selection screen)
    Replace Battle music with more duelists
        - Expand this to a spare space on the disk and make it more usable
        - Pegasus Toon
*/

// QOL ToDoMaybe fix movement issue for field fusions


uintptr_t ChangeReincarnationAmountPtr = 0x20228190;
uintptr_t LockTeamSelection = 0x2021c6f0; // lock the selection options
uintptr_t LockTeamSelection2 = 0x2021c6d8;
uintptr_t SelectWhiteDefault = 0x2021f160; //0x2021cda4;
uintptr_t SetoMainAI = 0x20117ca0; //Make seto only pass
uintptr_t MakeWhiteStart = 0x201a8f74; //Makes white always start
uintptr_t AllowRitualFusionsHand = 0x2026e694; //Allows non enchant fusions in hand
uintptr_t AllowRitualFusionField = 0x20247fa4; //Allows non enchant fusions on field
uintptr_t RemoveRngFromSlots = 0x202511b4;
uintptr_t ExpandZoom = 0x201d2c00;
uintptr_t NerfTerrain = 0x20266490;

// Music
uintptr_t AddMakosThemeToMako = 0X201aab68; // Play makos theme during Mako battle
uintptr_t BgmJumpFix1 = 0x201aab60;
uintptr_t BgmJumpFix2 = 0x201aab14;
uintptr_t BgmJumpFix3 = 0x201aab40;
uintptr_t BgmJumpFix4 = 0x201aab50;
uintptr_t ChangeBgmAddressTable1 = 0x202be6d0;
uintptr_t ChangeBgmAddressTable2 = 0x202be710;

//Remove negetive exp
//TODO Deckleader still loses exp on loss
uintptr_t negExpAtkLeader = 0x20274558;
uintptr_t negExpDelUnit = 0x2024f38c;
uintptr_t negExpDelUnit2 = 0x2024f404;
uintptr_t negExpDelUnit2_1 = 0x2024f460;
uintptr_t negExpDelUnit2_2 = 0x2024f4e4;


//Make space for more specific 3 in a row rares
uintptr_t MoveRaresPtr = 0x20251510;

//Allow 5 digits for LP
uintptr_t MoreDigitsOnScreen = 0x201b1bd0;

// Remove going to each unit when they arent going to be moved
uintptr_t improveAIInputOrig = 0x2027646c;
uintptr_t improveAIInputDest = 0x20360100;

//Probably wont use but good to know

uintptr_t ExpandSelectionRange = 0x201d2c94;
uintptr_t ExpandSelectionRange2 = 0x201d2cdc;
//mem::Patch((BYTE*)ExpandSelectionRange, (BYTE*)"\x0a\x00\x42\x28 ", 4);
//mem::Patch((BYTE*)ExpandSelectionRange2, (BYTE*)"\x0a\x00\x42\x28", 4);



void PatchNegetiveExpPatch()
{
    //TODO FIX
    printf("Removed loss of Exp\n");
    mem::Patch((BYTE*)negExpAtkLeader, (BYTE*)"\x00\xff\x07\x24", 4);
    mem::Patch((BYTE*)negExpDelUnit, (BYTE*)"\x00\xff\x07\x24", 4);
    mem::Patch((BYTE*)negExpDelUnit2, (BYTE*)"\x00\xff\x07\x24", 4);
    mem::Patch((BYTE*)negExpDelUnit2_1, (BYTE*)"\x00\xff\x07\x24", 4);
    mem::Patch((BYTE*)negExpDelUnit2_2, (BYTE*)"\x00\xff\x07\x24", 4);
}


void MakeSetoUseless()
{
    printf("Seto Useless\n");
    mem::NopMips((BYTE*)SetoMainAI, 104);
    //Makes seto's ai return early, and only passes
    mem::Patch((BYTE*)0x20117e40, (BYTE*)"\x00\x08\x02\x24", 4);
}

void ForceNewGameStartYorkists()
{
    printf("Forced White start campaign \n");
    //Loads team 1 into a1 register
    //change x01 to x00 to force game to start as Red
    mem::Patch((BYTE*)LockTeamSelection2, (BYTE*)"\x01\x00\x05\x24", 4);
    //Assigns has completed white side the value of a1
    mem::Patch((BYTE*)LockTeamSelection, (BYTE*)"\x02\x00\x25\xa2", 4);
    //changes your selected menu options to 0/ yorkists
    //leave this out to auto select red
    mem::Patch((BYTE*)SelectWhiteDefault, (BYTE*)"\xa8\x53\x20\xa0", 4);
}

void ChangeReincarnationAmount()
{
    printf("Change reincarnation amount\n");
    //First byte changes the reincarnation count
    mem::Patch((BYTE*)ChangeReincarnationAmountPtr, (BYTE*)"\x03\x00\xa5\x28", 4);
}

void AllowExpandedZoom()
{
    printf("Expanded Zoom\n");
    mem::Patch((BYTE*)ExpandZoom, (BYTE*)"\x71\x02\x41\x28\x04\x00\x20\x50\x71\x02\x03\x24", 12);
}

void WhiteRoseGoesFirst()
{
    printf("White Rose First \n");
    //Moves the first 7 instructions up 2 bytes to fill spares nops
    //Then the next two instructions(8th and 9th): first one loads 1 into s6, (change 0X,00,16,24 to change which team starts first)
    //Second one stores the value of s6 into s0 + 0xf92 which is TurnFirst variable
    mem::Patch((BYTE*)MakeWhiteStart,
                 (BYTE*)
                 "\x10\x28\x00\x00\x40\x79\x07\x0c\x01\x00\x04\x24\x3c\x01\x04\x8e\x40\x01\x05\x8e\x40\xb7\x06\x0c\x28\x36\x40\x72\x01\x00\x16\x24\x92\x0f\x16\xa2",
                 36); 
}

void AllowNonMonsterFusions()
{
    printf("Hand Fusions Pog\n");
    //both hackily reverse as if statement to allows fusions for any combination of cards that do not include equips.
    //(basically rewrites most of the function)
    mem::Patch((BYTE*)AllowRitualFusionsHand,
               (BYTE*)
               "\x02\x00\x06\x24\x00\x00\xe3\x92\x43\x21\x03\x00\x1e\x00\x86\x10\x00\x00\x00\x00\x00\x00\x43\x90\x43\x19\x03\x00\x1a\x00\x66\x10\x00\x00\x00\x00\x28\x26\x00\x72\x68\x25\x09\x0c\x28\x2e\x20\x72\xe7\x03\x03\x24\x0c\x00\x43\x14\x00\x22\x16\x00\x02\x00\x04\x24\x00\x12\x16\x00\x25\x18\x55\x00\x00\x00\x44\xae\x25\x10\x53\x00\x04\x00\x43\xae\x08\x00\x42\xae\xff\xff\x02\x24\x8b\x00\x00\x10\x0c\x00\x40\xae\x00\x22\x16\x00\x00\x00\x40\xae\x25\x18\x95\x00\x04\x00\x43\xae\x25\x18\x93\x00\x08\x00\x43\xae\x0c\x00\x42\xae\x82\x00\x00\x10\x28\x16\x00\x70\x3c\x00\x80\x14\x02\x00\x03\x24\x00\x00\x43\x90\x02\x00\x02\x24\x43\x19\x03\x00\x04\x00\x62\x10\x28\x26\xc0\x72\x2c\x00\x00\x10\x00\x00\x42\xae\x28\x26\xc0\x72\x28\x2e\xa0\x72\xf4\x23\x09\x0c\x28\x36\x20\x72\x0c\x00\x41\x04\x03\x1c\x02\x00\x02\x00\x04\x24\x00\x12\x16\x00\x25\x18\x55\x00\x00\x00\x44\xae\x25\x10\x53\x00\x04\x00\x43\xae\x08\x00\x42\xae\xff\xff\x02\x24\x69\x00\x00\x10\x0c\x00\x40\xae\x03\x1c\x02\x00\x0c\x00\x60\x54\x00\x12\x16\x00\x00\x22\x16\x00\x00\x00\x40\xae\x25\x18\x95\x00\x04\x00\x43\xae\x25\x18\x93\x00\x08\x00\x43\xae\xff\xff\x42\x30\x0c\x00\x42\xae\x5c\x00\x00\x10\x28\x16\x00\x70\x00\x12\x16\x00\x25\x18\x55\x00\x25\x10\x53\x00\x00\x1c\x03\x00\x25\x10\x62\x00\x01\x00\x03\x24\x00\x00\x43\xae\x50\x79\x07\x0c\x04\x00\x42\xae\x08\x00\x42\xae\x01\x00\x02\x24\x4f\x00\x00\x10\x0c\x00\x51\xae\x00\x00\x42\xae\x00\x1a\x16\x00\x25\x10\x75\x00\x04\x00\x42\xae\x25\x10\x73\x00\x08\x00\x42\xae\xff\xff\x02\x24\x46\x00\x00\x10\x0c\x00\x40\xae\x02\x00\x03\x24\x3b\x00\x83\x14\x00\x00\x00\x00\x00\x00\x42\x90\x43\x11\x02\x00\x04\x00\x40\x10\x28\x26\x80\x72\x2d\x00\x00\x10\x00\x00\x43\xae\x28\x26\x80\x72\x28\x2e\x60\x72\xf4\x23\x09\x0c\x28\x36\x00\x72\x0c\x00\x41\x04\x03\x1c\x02\x00\x02\x00\x04\x24\x00\x12\x16\x00\x25\x18\x55\x00\x00\x00\x44\xae\x25\x10\x53\x00\x04\x00\x43\xae\x08\x00\x42\xae\xff\xff\x02\x24\x2d\x00\x00\x10\x0c\x00\x40\xae\x03\x1c\x02\x00\x0c\x00\x60\x54\x00\x12\x16\x00\x00\x22\x16\x00\x00\x00\x40\xae\x25\x18\x95\x00\x04\x00\x43\xae\x25\x18\x93\x00\x08\x00\x43\xae\xff\xff\x42\x30\x0c\x00\x42\xae\x20\x00\x00\x10\x28\x16\x00\x70\x00\x12\x16\x00\x25\x18\x55\x00\x25\x10\x53\x00\x00\x1c\x03\x00\x25\x10\x62\x00\x01\x00\x03\x24\x00\x00\x43\xae\x50\x79\x07\x0c\x04\x00\x42\xae\x08\x00\x42\xae\x01\x00\x02\x3c\x0c\x00\x50\xae\x12\x00\x00\x10\x01\x00\x42\x34\x00\x00\x43\xae\x00\x1a\x16\x00\x25\x10\x75\x00\x04\x00\x42\xae\x25\x10\x73\x00\x08\x00\x42\xae\xff\xff\x02\x24\x09\x00\x00\x10\x0c\x00\x40\xae\x00\x00\x43\xae\x00\x1a\x16\x00\x25\x10\x75\x00\x04\x00\x42\xae\x25\x10\x73\x00\x08\x00\x42\xae\xff\xff\x02\x24\x0c\x00\x40\xae\x80\x00\xbf\x7b\x70\x00\xb7\x7b\x60\x00\xb6\x7b\x50\x00\xb5\x7b\x40\x00\xb4\x7b\x30\x00\xb3\x7b\x20\x00\xb2\x7b\x10\x00\xb1\x7b\x00\x00\xb0\x7b\x08\x00\xe0\x03\x90\x00\xbd\x27",
               696);
    printf("Field Fusions Double pog\n");
    mem::Patch((BYTE*)AllowRitualFusionField,
               (BYTE*)
               "\x02\x00\x19\x24\xc0\x81\x07\x0c\x00\x00\x00\x00\x43\x81\x02\x00\x28\x26\x60\x72\xc0\x81\x07\x0c\x28\x2e\x40\x72\x25\x00\x19\x12\x43\x19\x02\x00\x23\x00\x79\x10\x00\x00\x00\x00\x0d\x00\x80\x16\x28\x26\xa0\x72\x00\x12\x13\x00\x5f\x00\x04\x24\x25\x18\x52\x00\x00\x12\x15\x00\x00\x00\x24\xae\x25\x10\x54\x00\x04\x00\x23\xae\x08\x00\x22\xae\x01\x00\x02\x24\xd3\x00\x00\x10\x0c\x00\x22\xae\x28\x26\xa0\x72\x0c\x84\x07\x0c\x28\x2e\x80\x72\x28\x86\x40\x70\x28\x26\x60\x72\x0c\x84\x07\x0c\x28\x2e\x40\x72\x28\x26\x00\x72\x68\x25\x09\x0c\x28\x2e\x40\x70\x00\x1a\x15\x00\x64\x00\x05\x24\x25\x20\x74\x00\x00\x1a\x13\x00\x00\x00\x25\xae\x25\x18\x72\x00\x04\x00\x24\xae\x08\x00\x23\xae\x0c\x00\x22\xae\xbe\x00\x00\x10\x01\x00\x02\x24\x5b\x00\x00\x16\x02\x00\x02\x24\x02\x00\x02\x24\x57\x00\x62\x14\x00\x00\x00\x00\x0d\x00\x80\x16\x28\x26\x60\x72\x00\x12\x13\x00\x5f\x00\x04\x24\x25\x18\x52\x00\x00\x12\x15\x00\x00\x00\x24\xae\x25\x10\x54\x00\x04\x00\x23\xae\x08\x00\x22\xae\x01\x00\x02\x24\xac\x00\x00\x10\x0c\x00\x22\xae\x28\x26\x60\x72\x0c\x84\x07\x0c\x28\x2e\x40\x72\x28\x26\xa0\x72\x28\x2e\x80\x72\xbc\x22\x09\x0c\x28\x36\x40\x70\xe7\x03\x05\x24\x28\x86\x40\x70\x0d\x00\x05\x16\x83\x17\x10\x00\x00\x12\x15\x00\x24\x00\x04\x24\x25\x18\x54\x00\x00\x12\x13\x00\x00\x00\x24\xae\x25\x10\x52\x00\x04\x00\x23\xae\x08\x00\x22\xae\x01\x00\x02\x24\x96\x00\x00\x10\x0c\x00\x25\xae\x83\x17\x10\x00\x01\x00\x43\x30\x01\x00\x02\x24\x10\x00\x62\x14\x19\x02\x02\x24\x00\x12\x13\x00\x25\x20\x52\x00\x2b\x00\x05\x24\x00\x12\x15\x00\x25\x18\x54\x00\x00\x00\x25\xae\x83\x13\x10\x00\x04\x00\x24\xae\xff\x3f\x42\x30\x08\x00\x23\xae\x0c\x00\x22\xae\x01\x00\x02\x3c\x83\x00\x00\x10\x01\x00\x42\x34\x19\x02\x02\x24\x83\x1b\x10\x00\x12\x00\x62\x14\x00\x12\x15\x00\x25\x00\x03\x24\x00\x00\x23\xae\x00\x1a\x15\x00\x00\x12\x13\x00\x25\x18\x74\x00\x25\x10\x52\x00\x04\x00\x23\xae\x08\x00\x22\xae\x28\x26\xa0\x72\x9c\x37\x09\x0c\x28\x2e\x80\x72\x00\x1c\x02\x00\xff\x3f\x02\x32\x25\x10\x43\x00\x0a\x00\x00\x10\x0c\x00\x22\xae\x00\x12\x15\x00\x24\x00\x04\x24\x25\x18\x54\x00\x00\x12\x13\x00\x00\x00\x24\xae\x25\x10\x52\x00\x04\x00\x23\xae\x08\x00\x22\xae\x0c\x00\x30\xae\x01\x00\x02\x3c\x63\x00\x00\x10\x01\x00\x42\x34\x02\x00\x02\x24\x57\x00\x02\x16\x00\x12\x15\x00\x54\x00\x60\x14\x00\x00\x00\x00\x0d\x00\x40\x16\x28\x26\xa0\x72\x00\x12\x15\x00\x5f\x00\x04\x24\x25\x18\x54\x00\x00\x12\x13\x00\x00\x00\x24\xae\x25\x10\x52\x00\x04\x00\x23\xae\x08\x00\x22\xae\x01\x00\x02\x24\x51\x00\x00\x10\x0c\x00\x20\xae\x28\x26\xa0\x72\x0c\x84\x07\x0c\x28\x2e\x80\x72\x28\x26\x60\x72\x28\x2e\x40\x72\xbc\x22\x09\x0c\x28\x36\x40\x70\xe7\x03\x05\x24\x28\x86\x40\x70\x0d\x00\x05\x16\x83\x17\x10\x00\x00\x12\x15\x00\x24\x00\x04\x24\x25\x18\x54\x00\x00\x12\x13\x00\x00\x00\x24\xae\x25\x10\x52\x00\x04\x00\x23\xae\x08\x00\x22\xae\x01\x00\x02\x24\x3b\x00\x00\x10\x0c\x00\x25\xae\x83\x17\x10\x00\x01\x00\x43\x30\x01\x00\x02\x24\x0e\x00\x62\x54\x19\x02\x02\x24\x00\x1a\x15\x00\x25\x28\x74\x00\x2b\x00\x06\x24\x00\x1a\x13\x00\x25\x20\x72\x00\x00\x00\x26\xae\x83\x1b\x10\x00\x04\x00\x25\xae\xff\x3f\x63\x30\x08\x00\x24\xae\x2a\x00\x00\x10\x0c\x00\x23\xae\x19\x02\x02\x24\x83\x1b\x10\x00\x12\x00\x62\x14\x00\x12\x15\x00\x25\x00\x03\x24\x00\x12\x15\x00\x25\x10\x54\x00\x00\x00\x23\xae\x04\x00\x22\xae\x00\x12\x13\x00\x25\x10\x52\x00\x08\x00\x22\xae\x28\x26\x60\x72\x9c\x37\x09\x0c\x28\x2e\x40\x72\x00\x1c\x02\x00\xff\x3f\x02\x32\x25\x10\x43\x00\x0a\x00\x00\x10\x0c\x00\x22\xae\x00\x12\x15\x00\x24\x00\x04\x24\x25\x18\x54\x00\x00\x12\x13\x00\x00\x00\x24\xae\x25\x10\x52\x00\x04\x00\x23\xae\x08\x00\x22\xae\x0c\x00\x30\xae\x0b\x00\x00\x10\x01\x00\x02\x24\x00\x12\x15\x00\x5f\x00\x04\x24\x25\x18\x54\x00\x00\x12\x13\x00\x00\x00\x24\xae\x25\x10\x52\x00\x04\x00\x23\xae\x08\x00\x22\xae\x01\x00\x02\x24\x0c\x00\x20\xae\x60\x00\xbf\x7b\x50\x00\xb5\x7b\x40\x00\xb4\x7b\x30\x00\xb3\x7b\x20\x00\xb2\x7b\x10\x00\xb1\x7b\x00\x00\xb0\x7b\x08\x00\xe0\x03\x70\x00\xbd\x27",
               972);
}

void RemoveSlotsRng()
{
    printf("Remove Rng From Slots \n");
    //Assigns zero to the slots check for random field, when 0 this removes the random delay time after the button is clicked.
    //This is normally achieved when you destiny draw that game
    mem::Patch((BYTE*)RemoveRngFromSlots, (BYTE*)"\x01\x00\x60\xa2", 4);
}

void ChangeMakosTheme()
{
    printf("Add Mako's Theme To Mako\n");
    mem::Patch((BYTE*)AddMakosThemeToMako, (BYTE*)
               "\x0e\x00\x03\x24\x15\x00\xc3\x50\x23\x00\x06\x24\x14\x00\x03\x24\x15\x00\xc3\x50\x0c\x00\x06\x24\x13\x00\x03\x24\x0f\x00\xc3\x50\x0b\x00\x06\x24\x01\x00\x03\x24\x09\x00\xc3\x50\x0a\x00\x06\x24\x04\x00\xc0\x50\x07\x00\x06\x24\x10\x00\x00\x10\x16\x00\xc1\x28\x07\x00\x06\x24\x17\x00\x00\x10\x00\x00\x00\x00\x0a\x00\x06\x24\x14\x00\x00\x10\x00\x00\x00\x00\x0b\x00\x06\x24\x11\x00\x00\x10\x00\x00\x00\x00\x0c\x00\x06\x24\x0e\x00\x00\x10\x00\x00\x00\x00\x0c\x00\x00\x10\x2c\x00\x06\x24\x16\x00\xc1\x28\x09\x00\x20\x10\x28\x00\x06\x24\x04\x00\xe0\x10\x08\x00\x06\x24\x05\x00\x00\x10\x09\x00\x06\x24\x08\x00\x06\x24\x02\x00\x00\x10\x00\x00\x00\x00\x28\x00\x06\x24\x4d\x00\xc0\x04\x00\x00\x00\x00\x62\x10\x02\x3c\xd3\x4d\x42\x34\x18\x00\x44\x00\xc2\x1f\x04\x00\x00\x00\x00\x00\x10\x10\x00\x00\x83\x11\x02\x00\x21\x18\x43\x00\x40\x11\x03\x00\x23\x10\x43\x00\x80\x10\x02\x00\x21\x10\x43\x00\xc0\x10\x02\x00\x21\x30\xc2\x00\x60\x5e\x08\x0c\x28\x26\xc0\x70\x3b\x00\x00\x10\x00\x00\x00\x00\x4c\x01\xe3\x8c\x0a\x00\x61\x2c\x1b\x00\x20\x10\xff\xff\x04\x24\x2c\x00\x06\x3c\xd0\xe6\xc6\x24\x80\x18\x03\x00\x21\x18\x66\x00\x00\x00\x63\x8c\x08\x00\x60\x00\x00\x00\x00\x00\x12\x00\x00\x10\x0d\x00\x04\x24\x10\x00\x00\x10\x0e\x00\x04\x24\x0e\x00\x00\x10\x0f\x00\x04\x24\x0c\x00\x00\x10\x10\x00\x04\x24\x0a\x00\x00\x10\x11\x00\x04\x24\x08\x00\x00\x10\x12\x00\x04\x24\x06\x00\x00\x10\x13\x00\x04\x24\x04\x00\x00\x10\x14\x00\x04\x24\x02\x00\x00\x10\x15\x00\x04\x24\x16\x00\x04\x24\x1b\x00\x80\x04\x00\x00\x00\x00\x60\x5e\x08\x0c\x00\x00\x00\x00\x17\x00\x00\x10\x00\x00\x00\x00\x60\x5e\x08\x0c\x05\x00\x04\x24\x13\x00\x00\x10\x00\x00\x00\x00\x60\x5e\x08\x0c\x05\x00\x04\x24\x0f\x00\x00\x10\x00\x00\x00\x00\x60\x5e\x08\x0c\x29\x00\x04\x24\x0b\x00\x00\x10\x00\x00\x00\x00\x60\x5e\x08\x0c\x03\x00\x04\x24\x07\x00\x00\x10\x00\x00\x00\x00\x60\x5e\x08\x0c\x02\x00\x04\x24\x03\x00\x00\x10\x00\x00\x00\x00\x60\x5e\x08\x0c\x15\x00\x04\x24\x00\x00\xbf\x7b\x08\x00\xe0\x03\x10\x00\xbd\x27",
               488);
    mem::Patch((BYTE*)BgmJumpFix1, (BYTE*)"\x1d\x00\xc3\x10 ", 4);
    mem::Patch((BYTE*)BgmJumpFix2, (BYTE*)"\x8b\x00\x20\x10", 4);
    mem::Patch((BYTE*)BgmJumpFix3, (BYTE*)"\x81\x00\x00\x10", 4);
    mem::Patch((BYTE*)BgmJumpFix4, (BYTE*)"\x7c\x00\x00\x10", 4);
    mem::Patch((BYTE*)ChangeBgmAddressTable1,
               (BYTE*)
               "\xa8\xac\x1a\x00\xb0\xac\x1a\x00\x98\xac\x1a\x00\x90\xac\x1a\x00\xa0\xac\x1a\x00\xc0\xac\x1a\x00\xc8\xac\x1a\x00\x88\xac\x1a\x00\xd0\xac\x1a\x00\xb8\xac\x1a\x00",
               40);
    mem::Patch((BYTE*)ChangeBgmAddressTable2,
               (BYTE*)
               "\x44\xad\x1a\x00\x44\xad\x1a\x00\x44\xad\x1a\x00\x5c\xac\x1a\x00\xec\xac\x1a\x00\xfc\xac\x1a\x00\x0c\xad\x1a\x00\x1c\xad\x1a\x00\x2c\xad\x1a\x00\x44\xad\x1a\x00\x44\xad\x1a\x00\x44\xad\x1a\x00\x44\xad\x1a\x00\x3c\xad\x1a\x00",
               56);
}

void NerfTerrainTo300()
{
    printf("Terrain Nerfed\n");
    // D4(0th byte),FE(1st byte) = Negetive 300 change those bytes to change value of Negetive terrain
    // x2c(13th byte),x01 (14th byte) = postive 300 change those to change value of positive terrain
    mem::Patch((BYTE*)NerfTerrain, (BYTE*)"\xd4\xfe\xb5\x26\x01\x00\x03\x24\x04\x00\x43\x50\x2c\x01\xb5\x26", 16);
}

void MoveRares()
{
    printf("Moved Rares\n");
    //copies the start of specific three in a rows
    mem::MoveBytesMips((BYTE*)MoveRaresPtr, (BYTE*)0x20360000, 12);
    //Adds jump where it got copied from to its new location 
    mem::Patch((BYTE*)MoveRaresPtr, (BYTE*)"\x00\x80\x0d\x08", 4);

    //Fix jump offset for first check
    mem::Patch((BYTE*)0x20360004, (BYTE*)"\x06\x00\x62\x10\x00\x00\x00\x00", 8);

    //Adds  jump back if all fail
    //Maybe find a pattern to add these by number of cards

    // Adds new cards and jumps to end
    mem::Patch((BYTE*)0x2036000c,
               (BYTE*)
               "\x17\x02\x02\x24\x06\x00\x62\x10\x00\x00\x00\x00\x47\x45\x09\x08\x00\x00\x00\x00\x2b\x03\x02\x24\xab\x45\x09\x08\x00\x00\x00\x00\x1a\x02\x02\x24\xab\x45\x09\x08\x00\x00\x00\x00",
               44);
}
void ChangeStatCap()
{
    printf("Changed Stat Cap to 20k\n");

    //Changes stat cap function

    printf("Changed Stat Cap Func\n");
    //Changes the if check from 9999 to 20k
    mem::Patch((BYTE*)0x202508d4, (BYTE*)"\x21\x4e\x81\x28", 4);
    mem::Patch((BYTE*)0x202508e0, (BYTE*)"\x21\x4e\x81\x28", 4);
    //changes the assignment to 20k
    mem::Patch((BYTE*)0x202508ec, (BYTE*)"\x20\x4e\x04\x24", 4);
    
    //Changes SzDuel_CalcLP function
    printf("Changed Calc LP Func\n");
    //Changes the if check from 9999 to 20k
    mem::Patch((BYTE*)0x201df51c, (BYTE*)"\x21\x4e\x41\x28", 4);
    //changes the assignment to 20k
    mem::Patch((BYTE*)0x201df524, (BYTE*)"\x21\x4e\x41\x28", 4);
    
}

void Allow5DigitsOnUI()
{
    printf("Allows 5 digits on UI \n");

    ChangeStatCap();
    //Allow 5 digits on UI
    mem::Patch((BYTE*)MoreDigitsOnScreen, (BYTE*)"\x05\x00\x07\x24", 4);
    
    //Move player details box y pos
    mem::Patch((BYTE*)0x201b13b4, (BYTE*)"\x14\x00\x04\x24", 4);

    //Move Player name tag
    mem::Patch((BYTE*)0x201b1be4, (BYTE*)"\x94\x00\x03\x24 ", 4);
      	   


    //Move allignment (not needed anymore but could be useful in future)
    //mem::Patch((BYTE*)0x201b1c64, (BYTE*)"\x1c\x00\xf0\x26", 4);
    


    //offset white rose x pos left
    mem::Patch((BYTE*)0x201b13f8, (BYTE*)"\x90\x01\x06\x24 ", 4);

    //Move Player Picture Right
    //mem::Patch((BYTE*)0x201b1b34, (BYTE*)"\x18\x00\xa3\x26", 4);
    mem::Patch((BYTE*)0x201b1b2c, (BYTE*)"\x94\x00\x02\x24", 4);


    
    // make LP background box wider
    mem::Patch((BYTE*)0x201b1b9c, (BYTE*)"\x0e\x00\x08\x24", 4);



    //Make UI Wider
    mem::Patch((BYTE*)0x201b1afc, (BYTE*)"\x2a\x00\x0a\x24", 4);
    //Make UI Taller
    //mem::Patch((BYTE*)0x201b1b00, (BYTE*)"\x10\x00\x0b\x24", 4);
    	  
    
    
}

void AIPassInsteadOfPressXDeprecated()
{
    printf("Make AI Pass instead of x on no move units?\n");
    //moving bytes to the bottom to free up 3 free instrutions
    //mem::ShiftBytesMips((byte*)0x20276400,(byte*)0x2027640c,468);


    //Replace original code with jump to 0x20360100
    mem::Patch((BYTE*)improveAIInputOrig, (BYTE*)"\x40\x80\x0d\x08\x00\x00\x00\x00", 8);

    //One mega patch adds
    /*           if ((((task->shouldFlipUp == 2) && (colDist == 0)) && (rowDist == 0)) &&
         (task->atkDefPosition == 2)) {
        unit = SzDuel_GetUnit((int)(uint)task->unitOwnerId >> 8,task->unitOwnerId & 0xff);
        *(byte *)(int)unit->status = unit->status & 0xfeU | 1;
        rowDiff = 0;
        do {
          *(undefined2 *)((int)data->inputQue + rowDiff) = 0xffff;
          rowDiff = rowDiff + 2;
        } while (rowDiff < 0x40);
        goto LAB_00276b04;
      }
         */
    mem::Patch((BYTE*)improveAIInputDest, (BYTE*)"\x0c\x00\x03\x8e\x02\x00\x02\x24\x23\x00\x62\x14\x00\x00\x00\x00\x21\x00\xc0\x16\x00\x00\x00\x00\x00\x00\x00\x00\x1e\x00\xe0\x16\x00\x00\x00\x00\x10\x00\x02\x92\x02\x00\x03\x24\x1a\x00\x43\x14\x00\x00\x00\x00\x00\x00\x02\x96\x03\x22\x02\x00\x50\x7d\x07\x0c\xff\x00\x45\x30\x57\x00\x45\x80\xfe\x00\x07\x24\x24\x28\xe5\x00\x01\x00\xa5\x34\x57\x00\x45\xa0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1e\x24\xff\xff\x01\x34\x60\x01\x22\x26\x20\x10\x5e\x00\x00\x00\x41\xa4\x02\x00\xde\x27\x40\x00\xc2\x2b\x00\x00\x00\x00\xf9\xff\x40\x14\x00\x00\x00\x00\xc1\xda\x09\x08\x00\x00\x00\x00\x0c\x00\x03\x8e\x01\x00\x02\x24\x1d\xd9\x09\x08\x00\x00\x00\x00", 168);
    

    //Stop input removal?
    //mem::NopMips((BYTE*)0x20360168,9);
    
}


void AIPassInsteadOfPressX()
{
    //Patch Jump to 0036009c
    mem::Patch((BYTE*)0x20276300, (BYTE*)"\x27\x80\x0d\x08\x00\x00\x00\x00", 8);
    //Patch logic at jmp location
    //Broken
    mem::Patch((BYTE*)0x2036009c, (BYTE*)"\x02\x00\x03\x92\x06\x00\x0b\x24\x21\x00\x6b\x14\x00\x00\x00\x00\xff\x00\x4c\x30\x04\x00\x0d\x96\xff\x00\xb8\x31\x23\x70\x98\x01\x1b\x00\xc0\x15\x00\x00\x00\x00\x03\x62\x02\x00\x03\xc2\x0d\x00\x22\x70\x98\x01\x16\x00\xc0\x15\x00\x00\x00\x00\x00\x00\x09\x92\x00\x00\x00\x00\x00\x00\x00\x00\x0c\x00\x0d\x8e\x02\x00\x09\x24\x0f\x00\xa9\x15\x00\x00\x00\x00\x10\x00\x0d\x92\x0c\x00\xa9\x15\x00\x00\x00\x00\x50\x7d\x07\x0c\x00\x00\x00\x00\x57\x00\x4d\x80\x00\x00\x00\x00\x01\x00\xad\x35\x57\x00\x4d\xa0\x60\x01\x28\x26\x20\x00\x09\x24\x00\x00\x09\xa5\xc1\xda\x09\x08\x00\x00\x00\x00\xff\xff\x8a\x32\xff\x00\x83\x32\xc2\xd8\x09\x08", 156);
    
    
    
}



DWORD WINAPI ModThread(HMODULE hModule)
{
    //Create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Im in\n";

    // get module base
    uintptr_t baseModule = (uintptr_t)GetModuleHandle(L"pcsx2.exe");

    printf("Hooked\n");
    while (!GetAsyncKeyState(VK_END) & 1)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            MoveRares();
        }

        if (GetAsyncKeyState(VK_HOME) & 1)
        {
            printf("Patched\n");
            PatchNegetiveExpPatch();
            MakeSetoUseless();
            ForceNewGameStartYorkists();
            ChangeReincarnationAmount();
            AllowExpandedZoom();
            WhiteRoseGoesFirst();
            AllowNonMonsterFusions();
            RemoveSlotsRng();
            //ChangeMakosTheme();
            NerfTerrainTo300();
            Allow5DigitsOnUI();
            AIPassInsteadOfPressX();
        }
        Sleep(1);
    }

    //cleanup and eject
    if (f)
    {
        fclose(f);
    }
    FreeConsole();
    //Disable if hooking into another thread as main thread
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            HANDLE handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ModThread, hModule, 0, 0);
            if (handle)
            {
                CloseHandle(handle);
            }
        }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
