IconAssets = group{quality=8.8}
Icon = image{"icon.png"}

--[[
Music = tracker{"sv_ttt.xm"}
--]]

function frames(fmt, start, count, pingPong)
    t = {}
    for i = start, count do
        t[1+#t] = string.format(fmt, i)
    end
    if pingPong then
        for i = count-1, start+1, -1 do
            t[1+#t] = string.format(fmt, i)
        end
    end
    return t
end

BootstrapGroup = group{}

-- Loading bar (0, 1, 2, 3, 4, 5, 6, 7, 8)
LoadingBarPic = image{frames("images/bootstrap/loadingbar%04d.png", 1, 9), quality=10}

-- 8 random bg pic
BackGroundPic = image{frames("images/background/bg%04d.png", 1, 8), quality=9.98}

BootstrapGroup2 = group{}

-- the game label
GameLabelPic = image{"images/bootstrap/gamelabel.png", quality=9.89}

AlexSoftLabel = image{"images/alexsoft.png", quality=10}

MenuGroup = group{quality=9.955}

MenuBG = image{"images/menu/bg.png", pinned=1, quality=10}
MenuFooter = image{"images/menu/footer.png", quality=10}
MenuHeader = image{"images/menu/header.png", quality=10}

MenuNewGame = image{"images/menu/face0001.png"}
MenuNewGame_lbl = image{"images/menu/title0001.png", quality=10}

MenuResumeSaved = image{"images/menu/face0002.png"}
MenuResumeSaved_lbl = image{"images/menu/title0002.png", quality=10}

MenuDeleteSaved = image{"images/menu/face0003.png"}
MenuDeleteSaved_lbl = image{"images/menu/title0003.png", quality=10}

MenuPlayerCount = image{frames("images/menu/face%04d.png", 26, 36), quality=9.5}
MenuPlayerCount_lbl = image{"images/menu/title0026.png", quality=10}

MenuWinMode = image{{"images/menu/face0004.png", "images/menu/face0005.png"}}
MenuWinMode_lbl = image{"images/menu/title0004.png", quality=10}

MenuWinScore = image{frames("images/menu/face%04d.png", 7, 25), quality=9.5}
MenuWinScore_lbl = image{"images/menu/title0007.png", quality=10}

MenuOK = image{{"images/menu/face0037.png", "images/menu/face0006.png"}}
MenuOK_lbl = image{"images/menu/title0006.png", quality=10}

GameGroup = group{}

-- 55 pics (0~54) for cards and one cardbg
CardPic = image{frames("images/card/card%04d.png", 1, 55), quality=9.957}

-- A blank pic to clean bg1 layer
BlankCard = image{"images/card/blankcard.png", quality=10}

-- Arrow L/R for sprite layer
ArrowLRPic = image{{"images/l.png", "images/r.png"}, quality=10, pinned=true}

-- Bubble, shows the color for a wild card
BubblePic = image{"images/bubble.png", quality=9.5}
WildColorPic = image{frames("images/mini/mini%04d.png", 13, 16), quality=10}

-- Arrow Down for select color
ArrowDownPic = image{"images/down.png", quality=10, pinned=true}

-- How many cards are remaining
CardsRemainingPic = image{frames("images/mini/mini%04d.png", 0, 10), quality=8.5}

-- Now Player, next turn L/R
ArrowNowPlayerPic = image{frames("images/mini/mini%04d.png", 11, 12), quality=8.5}

-- Numbers for +2, +4
NumbersPic = image{"images/numbers.png", quality=10, pinned=true, width=16}

-- Popup animation
ReversePopupPic = image{"images/reverse_popup.png", quality=10, pinned=true}
SkipPopupPic = image{"images/skip_popup.png", quality=10, pinned=true}
UNOPopupPic = image{"images/uno_popup.png", quality=9.9, pinned=true}

-- Challenge / Cancel
ChallengeSelectPic = image{"images/challenge.png", quality=10}
