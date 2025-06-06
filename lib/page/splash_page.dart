import 'package:flutter/material.dart';
import 'package:plant_app/core/color.dart';
import 'package:plant_app/widgets/bottom_nav.dart';

class SplashPage extends StatelessWidget {
  const SplashPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Column(
        mainAxisAlignment: MainAxisAlignment.spaceAround,
        children: [
          const SizedBox(height: 25),
          const Text(
            'Let\'s control plant with us',
            style: TextStyle(
              fontSize: 22.0,
              letterSpacing: 1.8,
              fontWeight: FontWeight.w900,
            ),
          ),
          const SizedBox(height: 5),
          const Text(
            'Bring nature home',
            style: TextStyle(
              color: grey,
              fontSize: 16,
              letterSpacing: 1.8,
              fontWeight: FontWeight.w600,
            ),
          ),
          SizedBox(
            height: 450,
            width: 450,
            child: Image.asset('assets/images/Asset1.png'),
          ),
          const SizedBox(height: 25),
          GestureDetector(
            onTap: () {
              Navigator.push(
                  context,
                  MaterialPageRoute(
                      builder: (context) => const BottomNavBar()));
            },
            child: Container(
              padding: const EdgeInsets.symmetric(
                horizontal: 80.0,
                vertical: 12.0,
              ),
              decoration: BoxDecoration(
                color: green,
                borderRadius: BorderRadius.circular(10.0),
              ),
              child: const Text(
                'Go To Home',
                style: TextStyle(
                  color: white,
                  fontSize: 16,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ),
          ),
          // TextButton(
          //   onPressed: () {},
          //   child: Text(
          //     'Create an account',
          //     style: TextStyle(
          //       color: black.withOpacity(0.7),
          //       fontSize: 16,
          //       letterSpacing: 1,
          //       fontWeight: FontWeight.w600,
          //     ),
          //   ),
          // ),
          // TextButton(
          //   onPressed: () {},
          //   child: Text(
          //     'Forgot Password?',
          //     style: TextStyle(
          //       color: black.withOpacity(0.4),
          //       letterSpacing: 1,
          //       fontWeight: FontWeight.w600,
          //     ),
          //   ),
          // ),
        ],
      ),
    );
  }
}
