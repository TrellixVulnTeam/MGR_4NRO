   c   l   c   ;      
   c   l   e   a   r       a   l   l   ;      
      
   i   t   1       =       [   ]   ;      
   i   t   2       =       [   ]   ;      
   i   t   3       =       [   ]   ;      
      
   n   o   r   m   s   1       =       [   ]   ;      
   n   o   r   m   s   2       =       [   ]   ;      
   n   o   r   m   s   3       =       [   ]   ;      
   g   l   o   b   a   l       x   _   ;      
   g   l   o   b   a   l       d   _   ;      
   n       =       1   0   0   ;      
   f   o   r       k   =   4   :   1   0      
                   b   a   s   e       =       3   ;      
                   Q       =       z   e   r   o   s   (   1   ,   n   )   ;      
                   s   p   l   i   t       =       r   o   u   n   d   (   n   /   k   )   ;      
                   m       =       1   ;      
                   f   o   r       i   =   1   :   n      
                                   i   f       i   >   m   *   s   p   l   i   t      
                                                   m   =   m   +   1   ;      
                                   e   n   d      
                                   Q   (   1   ,   i   )   =   b   a   s   e   *   m   ;      
                   e   n   d      
                   [   A   ,   b   ]       =       W   y   g   e   n   e   r   u   j   M   a   c   i   e   r   z   W   a   r   t   o   s   c   i   W   l   a   s   n   e   (   Q   )   ;      
                      
                   [   f   ,   d   f   ,   ~   ]       =       f   u   n   (   A   ,   b   )   ;      
                      
                   x   _       =       z   e   r   o   s   (   n   ,   1   )   ;      
                   d   _   =   -   d   f   (   x   _   )   ;      
                      
                   a   l   f   a   f   u   n       =       @   (   a   l   f   a   )   (   f   (   x   _   +   a   l   f   a   *   d   _   )   )   ;      
                   a   l   f   a   f   u   n   _   d       =       @   (   a   l   f   a   )   (   d   f   (   x   _   +   a   l   f   a   *   d   _   )   )   ;      
                      
                   x   0       =       z   e   r   o   s   (   n   ,   1   )   ;      
                   x   E   x   a   c   t       =       A   \   b   ;      
                   [   x   1   ,   ~   ,   i   t   F   r   1   ,   ~   ]       =       F   R   (   a   l   f   a   f   u   n   ,   a   l   f   a   f   u   n   _   d   ,   d   f   ,   x   0   ,   1   e   -   5   ,   1   ,   1   ,   0   )   ;      
                   i   t   1       =       [   i   t   1       i   t   F   r   1   ]   ;      
                   [   x   2   ,   ~   ,   i   t   F   r   2   ,   ~   ]       =       F   R   (   a   l   f   a   f   u   n   ,   a   l   f   a   f   u   n   _   d   ,   d   f   ,   x   0   ,   1   e   -   5   ,   2   ,   1   ,   0   )   ;      
                   i   t   2       =       [   i   t   2       i   t   F   r   2   ]   ;      
                   [   x   3   ,   ~   ,   i   t   F   r   3   ,   ~   ]       =       F   R   (   a   l   f   a   f   u   n   ,   a   l   f   a   f   u   n   _   d   ,   d   f   ,   x   0   ,   1   e   -   5   ,   3   ,   1   ,   0   )   ;      
                   i   t   3       =       [   i   t   3       i   t   F   r   3   ]   ;      
                   n   o   r   m   s   1       =       [   n   o   r   m   s   1       n   o   r   m   (   x   1   -   x   E   x   a   c   t   )   ]   ;      
                   n   o   r   m   s   2       =       [   n   o   r   m   s   3       n   o   r   m   (   x   2   -   x   E   x   a   c   t   )   ]   ;      
                   n   o   r   m   s   3       =       [   n   o   r   m   s   3       n   o   r   m   (   x   3   -   x   E   x   a   c   t   )   ]   ;      
   e   n   d      
   f   i   g   u   r   e   ;      
   n   =   4   :   1   0   ;      
   p   l   o   t   (   n   ,   i   t   1   ,   '   r   '   ,   n   ,   i   t   2   ,   '   g   '   ,   n   ,   i   t   3   ,   '   b   '   )   ;      
   x   l   a   b   e   l   (   '   L   i   c   z   b   a       k   l   a   s   t   r   �   w   '   )      
   y   l   a   b   e   l   (   '   L   i   c   z   b   a       i   t   e   r   a   c   j   i   '   )      
      
   n       =       1   0   0   ;      
   Q       =       3       *       o   n   e   s   (   1   ,   n   )   ;      
   Q   (   1   ,   n   )       =       3   0   ;          
   [   A   ,   b   ]       =       W   y   g   e   n   e   r   u   j   M   a   c   i   e   r   z   W   a   r   t   o   s   c   i   W   l   a   s   n   e   (   Q   )   ;      
   [   f   ,   d   f   ,   d   d   f   ]       =       f   u   n   (   A   ,   b   )   ;      
   x   _       =       z   e   r   o   s   (   n   ,   1   )   ;      
   d   _   =   -   d   f   (   x   _   )   ;      
   a   l   f   a   0       =       0   ;      
   a   l   f   a   f   u   n       =       @   (   a   l   f   a   )   (   f   (   x   _   +   a   l   f   a   *   d   _   )   )   ;      
   a   l   f   a   f   u   n   _   d       =       @   (   a   l   f   a   )   (   d   f   (   x   _   +   a   l   f   a   *   d   _   )   )   ;      
   a   l   f   a   m   a   x       =       a   l   f   a   _   m   a   x   (   a   l   f   a   f   u   n   ,   a   l   f   a   0   )   ;      
   x   0       =       z   e   r   o   s   (   n   ,   1   )   ;      
   f   u   n   _   a   n   d   _   g   r   a   d       =       @   (   x   )       d   e   a   l   (   f   (   x   )   ,   d   f   (   x   )   )   ;      
   [   x   F   r   1   ,   ~   ,   ~   ,   g   r   N   o   r   m   s   1   ]       =       F   R   (   a   l   f   a   f   u   n   ,   a   l   f   a   f   u   n   _   d   ,   d   f   ,   x   0   ,   1   e   -   5   ,   1   ,   1   ,   1   )   ;      
   [   x   F   r   2   ,   ~   ,   ~   ,   g   r   N   o   r   m   s   2   ]       =       F   R   (   a   l   f   a   f   u   n   ,   a   l   f   a   f   u   n   _   d   ,   d   f   ,   x   0   ,   1   e   -   5   ,   2   ,   1   ,   1   )   ;      
   [   x   F   r   3   ,   ~   ,   ~   ,   g   r   N   o   r   m   s   3   ]       =       F   R   (   a   l   f   a   f   u   n   ,   a   l   f   a   f   u   n   _   d   ,   d   f   ,   x   0   ,   1   e   -   5   ,   3   ,   1   ,   1   )   ;      
   f   i   g   u   r   e   ;      
   s   e   m   i   l   o   g   y   (   1   :   l   e   n   g   t   h   (   g   r   N   o   r   m   s   1   )   ,   g   r   N   o   r   m   s   1   ,   '   r   '   ,   1   :   l   e   n   g   t   h   (   g   r   N   o   r   m   s   2   )   ,   g   r   N   o   r   m   s   2   ,   '   g   '   ,   1   :   l   e   n   g   t   h   (   g   r   N   o   r   m   s   3   )   ,   g   r   N   o   r   m   s   3   ,   '   b   '   )   ;      
   x   l   a   b   e   l   (   '   I   t   e   r   a   c   j   a   '   )      
   y   l   a   b   e   l   (   '   N   o   r   m   a   '   )