// Generated by Lona Compiler 0.6.0

import React from "react"
import { Image, Text, View, StyleSheet, TextStyles } from "react-sketchapp"

import colors from "../foundation/colors"
import shadows from "../foundation/shadows"
import textStyles from "../foundation/textStyles"

export default class OpacityTest extends React.Component {
  render() {


    let View$opacity
    View$opacity = 1

    if (this.props.selected) {

      View$opacity = 0.7
    }
    return (
      <View style={[styles.view, { opacity: View$opacity }]}>
        <View style={styles.view1}>
          <Text style={styles.text}>
            {"Text goes here"}
          </Text>
          <Image
            style={styles.image}
            source={require("../assets/icon_128x128.png")}

          />
        </View>
      </View>
    );
  }
};

let styles = StyleSheet.create({
  view: {
    alignItems: "flex-start",
    alignSelf: "stretch",
    backgroundColor: colors.blue500,
    flex: 0,
    flexDirection: "column",
    justifyContent: "flex-start",
    borderColor: colors.pink300,
    borderStyle: "solid",
    borderWidth: 10
  },
  view1: {
    alignItems: "flex-start",
    backgroundColor: colors.red900,
    opacity: 0.8,
    flexDirection: "column",
    justifyContent: "flex-start",
    width: 100,
    height: 100
  },
  text: {
    ...TextStyles.get("body1"),
    alignItems: "flex-start",
    opacity: 0.8,
    flex: 0,
    flexDirection: "column",
    justifyContent: "flex-start"
  },
  image: {
    alignItems: "flex-start",
    opacity: 0.5,
    flexDirection: "column",
    justifyContent: "flex-start",
    width: 90,
    height: 60,
    resizeMode: "cover"
  },
  imageResizeModeCover: {
    width: "100%",
    height: "100%",
    resizeMode: "cover",
    position: "absolute"
  }
})