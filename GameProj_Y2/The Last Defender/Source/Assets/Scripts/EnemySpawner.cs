using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using EngineScripting;

public class EnemySpawner : EID
{
	EnemySpawner()
	{
	}

    // This method is called once when the script is first initialized
    public void OnCreate()
    {
    }

    // This method is called once per frame
    void OnUpdate(float dt)
    {
        // Update logic here
        // Use the game timer or smth
        if (Input.GetKeyReleased(KeyCode.K))
        {
            Debug.Log("sahdiashfoeihfe");
            CheatEngine.SpawnEnemyWave();
        }
    }
}
