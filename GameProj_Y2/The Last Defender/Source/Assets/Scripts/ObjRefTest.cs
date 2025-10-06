using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using EngineScripting;

public class ObjRefTest : EID
{
    // This needs to save the address cause this is the address of the entity
    // NOT its uid inside cpp
    UInt64 GameObjectRef;
    GameObject reactorRef;

    ObjRefTest()
	{
	}

    // This method is called once when the script is first initialized
    public void OnCreate()
    {
    }

    void OnStart()
    {
        reactorRef = new GameObject(GameObjectRef);
    }

    // This method is called once per frame
    void OnUpdate(float dt)
    {
        // Update logic here
        if (Input.GetKeyReleased(KeyCode.J))
        {
            reactorRef.transform.localPosition = new Vector2(0, 0);
        }
    }
}
