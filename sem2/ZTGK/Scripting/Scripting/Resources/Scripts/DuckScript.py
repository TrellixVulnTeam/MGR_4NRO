from API import Vector3
import random
import time

class DuckScript:
    def __init__(self):
        self.gameObject = None
        pass

    def OnCreate(self):
        self.gameObject.SetScale(Vector3(0.01,0.01,0.01))
        pass

    def OnUpdate(self):
        pos = self.gameObject.GetPosition()
        nextPos = pos + Vector3(0.01,0,0.01)
        nextPos2 = nextPos + Vector3(0.01,0,0.01)
        self.gameObject.SetPosition(nextPos)
        self.gameObject.SetNextPosition(nextPos2)
        pass