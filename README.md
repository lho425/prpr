prpr
====

pretty proxy - a generic purpose proxy -

[ja]

汎用プロクシサーバ

簡単な使い方  
prpr port command arg1 arg2 ...

portにクライアントが接続に来たらforkしてcommand arg1 arg2 ... をexecする。  
execされたコマンドの標準入出力はTCPやUDPのソケットに繋がれる。 

例  
prpr 8888 cat  
#これはエコーバックサーバ  


コンパイルの仕方(例)  
gcc -O3 prpr.c -o prpr  

このソフトは自分の用途に合わせて、自由に改造するという使い方ができます。

このソフトと文章はパブリックドメインで配布されています
