// Generated by Lona Compiler 0.6.0

import React from "react"
import styled from "styled-components"

import colors from "../foundation/colors"
import shadows from "../foundation/shadows"
import textStyles from "../foundation/textStyles"
import LocalAsset from "../images/LocalAsset"

export default class NestedBottomLeftLayout extends React.Component {
  render() {



    return <View> <View1> <LocalAsset /> </View1> </View>;
  }
};

let View = styled.div({
  alignItems: "flex-start",
  display: "flex",
  flex: "1 1 0%",
  flexDirection: "column",
  justifyContent: "flex-start"
})

let View1 = styled.div({
  alignItems: "flex-end",
  backgroundColor: colors.red100,
  display: "flex",
  flexDirection: "row",
  justifyContent: "flex-start",
  width: "150px",
  height: "150px"
})