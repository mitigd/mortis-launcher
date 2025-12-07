#ifndef GAMEDATABASE_H
#define GAMEDATABASE_H

#include <string>
#include <map>

namespace Core
{
    struct GameDatabase
    {
        static const std::map<std::string, std::string> &GetDreammMap()
        {
            static const std::map<std::string, std::string> idToName = {

                // Afterlife
                {"lec-alife", "Afterlife"},

                // Battlehawks: 1942
                {"lec-battlehawks", "Battlehawks: 1942"},

                // Star Wars: Behind the Magic
                {"lec-behindmagic", "Star Wars: Behind the Magic"},

                // Ben’s Game
                {"lec-bensgame", "Ben's Game"},

                // Curse of Monkey Island
                {"lec-comi", "Curse of Monkey Island"},

                // FM Towns Multi‑Game Demos
                {"lec-demos-fmtowns", "Star Wars: Dark Forces"},

                // Star Wars: Dark Forces
                {"lec-darkforces", "Star Wars: Dark Forces"},

                // Day of the Tentacle
                {"lec-dott", "Day of the Tentacle"},

                // Escape from Monkey Island
                {"lec-efmi", "Escape from Monkey Island"},

                // Star Wars: Episode I Insider's Guide
                {"lec-ep1insider", "Star Wars: Episode I Insider's Guide"},

                // Their Finest Hour: Battle of Britain
                {"lec-finest", "Their Finest Hour: Battle of Britain"},

                // Grim Fandango
                {"lec-grim", "Grim Fandango"},

                // Indiana Jones and the Last Crusade
                {"lec-indy3", "Indiana Jones and the Last Crusade"},

                // Indiana Jones and the Last Crusade: The Action Game
                {"lec-indy3-action", "Indiana Jones and the Last Crusade: The Action Game"},

                // Indiana Jones and the Fate of Atlantis
                {"lec-indy4", "Indiana Jones and the Fate of Atlantis"},

                // Indiana Jones and the Fate of Atlantis: The Action Game
                {"lec-indy4-action", "Indiana Jones and the Fate of Atlantis: The Action Game"},

                // Indiana Jones and His Desktop Adventures
                {"lec-indydesk", "Indiana Jones and His Desktop Adventures"},

                // Indiana Jones and the Infernal Machine
                {"lec-infernal", "Indiana Jones and the Infernal Machine"},

                // Star Wars: Jedi Knight
                {"lec-jedi", "Star Wars: Jedi Knight"},

                // Loom
                {"lec-loom", "Loom"},

                // LucasArts Collection
                {"lec-collections", "Loom"},

                // Star Wars: Making Magic
                {"lec-makingmagic", "Star Wars: Making Magic"},

                // Maniac Mansion
                {"lec-maniac", "Maniac Mansion"},

                // Masterblazer
                {"lec-masterblazer", "Masterblazer"},

                // Monkey Island 2: LeChuck's Revenge
                {"lec-monkey2", "Monkey Island 2: LeChuck's Revenge"},

                // Monopoly Star Wars
                {"has-swmonopoly", "Monopoly Star Wars"},

                // Mortimer and the Riddles of the Medallion
                {"lec-mortimer", "Mortimer and the Riddles of the Medallion"},

                // Star Wars: Jedi Knight: Mysteries of the Sith
                {"lec-mots", "Star Wars: Jedi Knight: Mysteries of the Sith"},

                // Night Shift
                {"lec-nightshift", "Night Shift"},

                // Outlaws
                {"lec-outlaws", "Outlaws"},

                // Passport to Adventure
                {"lec-passport", "Passport to Adventure"},

                // Star Wars: Episode I The Phantom Menace
                {"lec-phantom", "Star Wars: Episode I The Phantom Menace"},

                // PHM Pegasus
                {"lec-phmpegasus", "PHM Pegasus"},

                // Pipe Dream
                {"lec-pipedream", "Pipe Dream"},

                // Star Wars: Episode I Racer
                {"lec-racer", "Star Wars: Episode I Racer"},

                // Star Wars: Rebel Assault
                {"lec-rebelassault", "Star Wars: Rebel Assault"},

                // Star Wars: Rebel Assault II
                {"lec-rebel2", "Star Wars: Rebel Assault II"},

                // Star Wars: Rebellion
                {"lec-rebellion", "Star Wars: Rebellion"},

                // Star Wars: Rogue Squadron
                {"lec-roguesq", "Star Wars: Rogue Squadron"},

                // Sam & Max Hit the Road
                {"lec-samnmax", "Sam & Max Hit the Road"},

                // Star Wars: Shadows of the Empire
                {"lec-shadows", "Star Wars: Shadows of the Empire"},

                // The Secret of Monkey Island
                {"lec-somi", "The Secret of Monkey Island"},

                // Super Star Wars (Prototype)
                {"lec-sswproto", "Super Star Wars (Prototype)"},

                // Strike Fleet
                {"lec-strikefleet", "Strike Fleet"},

                // Secret Weapons of the Luftwaffe
                {"lec-swotl", "Secret Weapons of the Luftwaffe"},

                // Star Wars: Screen Entertainment
                {"lec-swse", "Star Wars: Screen Entertainment"},

                // The Dig
                {"lec-thedig", "The Dig"},

                // Full Throttle
                {"lec-throttle", "Full Throttle"},

                // Star Wars: TIE Fighter
                {"lec-tie", "Star Wars: TIE Fighter"},

                // Star Wars: X-Wing vs. TIE Fighter
                {"lec-xvt", "Star Wars: X-Wing vs. TIE Fighter"},

                // Star Wars: X-Wing Alliance
                {"lec-xwa", "Star Wars: X-Wing Alliance"},

                // Star Wars: X-Wing
                {"lec-xwing", "Star Wars: X-Wing"},

                // Yoda Stories
                {"lec-yoda", "Yoda Stories"},

                // Zak McKracken and the Alien Mindbenders
                {"lec-zak", "Zak McKracken and the Alien Mindbenders"},

                // Star Wars: Anakin's Speedway
                {"lll-anakin", "Star Wars: Anakin's Speedway"},

                // Star Wars: DroidWorks
                {"lll-droidworks", "Star Wars: DroidWorks"},

                // Star Wars: Early Learning Activity Center
                {"lll-elac", "Star Wars: Early Learning Activity Center"},

                // Star Wars: Episode I The Gungan Frontier
                {"lll-gungan", "Star Wars: Episode I The Gungan Frontier"},

                // Star Wars: Jar Jar's Journey
                {"lll-jarjar", "Star Wars: Jar Jar's Journey"},

                // Star Wars: Pit Droids
                {"lll-pitdroids", "Star Wars: Pit Droids"},

                // Star Wars Math: Jabba's Game Galaxy
                {"lll-swmath", "Star Wars Math: Jabba's Game Galaxy"},

                // Star Wars: Yoda's Challenge Activity Center
                {"lll-yoda", "Star Wars: Yoda's Challenge Activity Center"},

                // Indiana Jones and the Temple of Doom
                {"mind-indy2", "Indiana Jones and the Temple of Doom"},

                // Willow
                {"mind-willow", "Willow"},

                // Star Wars Chess
                {"swt-swchess", "Star Wars Chess"},

                // Tetris
                {"tetris", "Tetris"},

            };
            return idToName;
        }
    };
}

#endif // GAMEDATABASE_H