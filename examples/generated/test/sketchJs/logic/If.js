// Generated by Lona Compiler 0.6.0

import React from "react"
import { View, StyleSheet, TextStyles } from "react-sketchapp"

import colors from "../foundation/colors"
import shadows from "../foundation/shadows"
import textStyles from "../foundation/textStyles"

export default class If extends React.Component {
  render() {


    let View$backgroundColor
    View$backgroundColor = "transparent"

    if (this.props.enabled) {

      View$backgroundColor = colors.red500
    }
    return (
      <View style={[styles.view, { backgroundColor: View$backgroundColor }]} />
    );
  }
};

let styles = StyleSheet.create({
  view: {
    alignItems: "flex-start",
    alignSelf: "stretch",
    flex: 0,
    flexDirection: "column",
    justifyContent: "flex-start"
  }
})