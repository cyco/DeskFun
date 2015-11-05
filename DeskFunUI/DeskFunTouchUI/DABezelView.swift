//
//  DABezelView.swift
//  DeskFunUI
//
//  Copyright © 2015 ccl. All rights reserved.
//

import Foundation
import UIKit

public class DABezelView : UIView {
    public var bezelWidth: CGFloat = 2.0;
    public var invertColors: Bool = false;
    public var background: UIColor = DAWindowTheme.BackgroundColor;

    override public func drawRect(rect: CGRect) {
        let bounds = self.bounds;
        let maxX = CGRectGetMaxX(bounds);
        let maxY = CGRectGetMaxY(bounds);

        let innerColor = invertColors ? DAWindowTheme.BezelLightColor : DAWindowTheme.BezelDarkColor;
        let outerColor = invertColors ? DAWindowTheme.BezelDarkColor : DAWindowTheme.BezelLightColor;

        var path = UIBezierPath();
        path.moveToPoint(CGPointMake(0, maxY));
        path.addLineToPoint(CGPointMake(0, 0));
        path.addLineToPoint(CGPointMake(maxX,0));
        path.addLineToPoint(CGPointMake(maxX-bezelWidth, bezelWidth));
        path.addLineToPoint(CGPointMake(bezelWidth, bezelWidth));
        path.addLineToPoint(CGPointMake(bezelWidth, maxY-bezelWidth));
        path.addLineToPoint(CGPointMake(0, maxY));
        innerColor.setFill();
        path.fill();

        path = UIBezierPath();
        path.moveToPoint(CGPointMake(0, maxY));
        path.addLineToPoint(CGPointMake(maxX,maxY));
        path.addLineToPoint(CGPointMake(maxX,0));
        path.addLineToPoint(CGPointMake(maxX-bezelWidth,bezelWidth));
        path.addLineToPoint(CGPointMake(maxX-bezelWidth,maxY-bezelWidth));
        path.addLineToPoint(CGPointMake(bezelWidth,maxY-bezelWidth));
        path.addLineToPoint(CGPointMake(0, maxY));

        outerColor.setFill();
        path.fill();

        background.setFill();
        UIRectFill(CGRectInset(bounds, bezelWidth, bezelWidth));
    }
}