# Helpless_guy
Helpless_guy

游戏流程分为两阶段 process ：
1.接收指令，
2.播放动画，


Player(人偶) 每5到10 秒 接受一次 指令

1.接收指令：
画面通知玩家，现在可以移动marker，给player下达指令，  
本阶段持续5秒，假设5秒内接收20个picture  
分析前后两个picture的marker的移动方向，共19个方向。  
19个方向中，出现最多的方向就是player的移动方向  
然后分析前后两个picture的marker的移动距离，共19个方向，  
如果有一个移动距离过大，则判断为 过快移动 ，player摔倒  
如果所有移动距离都过小，则判断为 原地不动  
11


Function   
name:GetPictureFromCamera  
Input:  
Outpunt:Picture  
Description: return a picture from Camera  


name:TrackingMarker  
Input:Picture  
Outpunt: PositionOfMarker  
Description: track the makrder and get the position of the marker  

由于稳定度问题，marker position 是动态变化的  

name:isPositionChanged  
Input:PositionOfMarker  
Outpunt: True/False  
Description: 如果所有移动距离都过小，则判断为 原地不动，返回False，  

name:isPositionOverChanged  
Input:PositionOfMarker  
Outpunt: True/False  
Description: 如果有一个移动距离过大，则判断为 过快移动 ，返回True  

name:DirectionOfMarker  
Input:PositionOfMarker  
Outpunt:direction  
Description:分析前后两个picture的marker的移动方向，共19个方向。  
19个方向中，出现最多的方向就是player的移动方向  

name: isGoEast  
Input:PositionOfMarker  
Outpunt:True/False  
Description:分析前后两个picture的marker的移动方向 是否向East  

name: isGoWest  
Input:PositionOfMarker  
Outpunt:True/False  
Description:分析前后两个picture的marker的移动方向 是否向West  

name: isGoSouth  
Input:PositionOfMarker  
Outpunt:True/False  
Description:分析前后两个picture的marker的移动方向 是否向South  

name: isGoNorth  
Input:PositionOfMarker  
Outpunt:True/False  
Description:分析前后两个picture的marker的移动方向 是否向north  

name:isGoUp  
Input:PositionOfMarker  
Outpunt:True/False  
Description:分析前后两个picture的marker的移动方向 是否向Up  

name:isGoDown  
Input:PositionOfMarker    
Outpunt:True/False  
Description:分析前后两个picture的marker的移动方向 是否向Down  

name:playVideo  
Input:direction  
Outpunt:  
Description:根据direction播放video  
