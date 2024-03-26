import 'package:flutter/material.dart';
import 'AdminMap.dart'; // Importing the AdminMap widget
import 'create_account_screen.dart'; // Importing the create account screen

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Fishing Boat Monitoring',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: WelcomeScreen(),
    );
  }
}

class WelcomeScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Row(
          children: [
            Icon(Icons.directions_boat), // Fishing boat icon
            SizedBox(width: 8), // Add some spacing
            Text('Fishing Boat Monitoring App'), // App name
          ],
        ),
      ),
      body: Container(
        color: Color(0xFFB5EDF4), // Color B5EDF4
        child: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Icon(Icons.directions_boat, size: 100), // Fishing boat icon
              SizedBox(height: 10), // Add spacing between the icon and text
              Text(
                'WELCOME',
                style: TextStyle(
                  fontSize: 24,
                  fontWeight: FontWeight.bold,
                  color: Colors.black,
                  letterSpacing:
                      2.0, // Increase letter spacing for a better look
                ),
              ),
              SizedBox(
                  height: 10), // Add spacing between "WELCOME" and the buttons
              SizedBox(
                width: 200, // Adjust the width to your desired length
                child: ElevatedButton.icon(
                  onPressed: () {
                    // Navigate to the Admin screen
                    Navigator.push(
                      context,
                      MaterialPageRoute(builder: (context) => AdminScreen()),
                    );
                  },
                  style: ButtonStyle(
                    backgroundColor: MaterialStateProperty.all<Color>(
                        Color(0xFF474C62)), // Color for Admin button
                  ),
                  icon: Icon(Icons.admin_panel_settings), // Admin icon
                  label: Text('Admin'),
                ),
              ),
              SizedBox(height: 10), // Add spacing between the buttons
              SizedBox(
                width: 200, // Adjust the width to your desired length
                child: ElevatedButton.icon(
                  onPressed: () {
                    // Navigate to the Create Account screen for Boat Owner
                    Navigator.push(
                      context,
                      MaterialPageRoute(
                          builder: (context) => CreateAccountScreen()),
                    );
                  },
                  style: ButtonStyle(
                    backgroundColor: MaterialStateProperty.all<Color>(
                        Color(0xFFD9D9D9)), // Color for Boat Owner button
                  ),
                  icon: Icon(Icons.directions_boat), // Boat icon
                  label: Text('Boat Owner'),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class AdminScreen extends StatelessWidget {
  const AdminScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Admin'),
      ),
      body: Column(
        children: [
          Expanded(
            child: AdminMap(), // Integrating AdminMap widget here
          ),
        ],
      ),
    );
  }
}
