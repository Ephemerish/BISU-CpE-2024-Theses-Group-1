// ignore_for_file: prefer_const_constructors, use_key_in_widget_constructors

import 'package:flutter/material.dart';

class CreateAccountScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Create Account'),
      ),
      body: Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            TextField(
              decoration: InputDecoration(labelText: 'Name'),
            ),
            SizedBox(height: 16.0),
            TextField(
              decoration: InputDecoration(labelText: 'Boat Number'),
            ),
            SizedBox(height: 16.0),
            ElevatedButton(
              onPressed: () {
                // Implement logic to save account information
                // For now, just pop the screen
                Navigator.pop(context);
              },
              child: Text('Register'),
            ),
          ],
        ),
      ),
    );
  }
}
