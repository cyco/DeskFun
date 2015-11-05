//
//  DAHealthView.swift
//  DeskFunUI
//
//  Copyright © 2015 ccl. All rights reserved.
//

import Foundation

public class DAHealthView : DABezelView
{
    public var goodColor = UIColor.greenColor();
    public var mediumColor = UIColor.yellowColor();
    public var badColor = UIColor.redColor();
    public var criticalColor = UIColor.blackColor();

    public var health :CGFloat = 3.0 {
        didSet {
            self.setNeedsDisplay();
        }
    }

    override init(frame: CGRect) {
        super.init(frame: frame);
        performInit();
    }

    required public init(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder);
        performInit();
    }

    private func performInit(){
        self.bezelWidth = 3.0;
    }

    override public func drawRect(rect: CGRect) {
        let bounds = self.bounds;
        let maxX = CGRectGetMaxX(bounds);
        let maxY = CGRectGetMaxY(bounds);
        let minY = CGRectGetMinY(bounds);
        let minX = CGRectGetMinX(bounds);
        let midX = CGRectGetMidX(bounds);
        let midY = CGRectGetMidY(bounds);
        let viewCenter = CGPointMake(midX, midY);

        let c = UIGraphicsGetCurrentContext();
        CGContextClearRect(c, bounds);
        CGContextSaveGState(c);

        UIBezierPath(ovalInRect: bounds).addClip();

        var split = UIBezierPath();
        split.moveToPoint(CGPointMake(minX, maxY));
        split.addLineToPoint(CGPointMake(maxX, minY));
        split.addLineToPoint(CGPointMake(maxX, maxY));
        split.addLineToPoint(CGPointMake(minX, maxY));

        DAWindowTheme.BezelLightColor.setFill();
        split.fill();

        split = UIBezierPath();
        split.moveToPoint(CGPointMake(minX, maxY));
        split.addLineToPoint(CGPointMake(minX, minY));
        split.addLineToPoint(CGPointMake(maxX, minY));
        split.addLineToPoint(CGPointMake(minX, maxY));

        DAWindowTheme.BezelDarkColor.setFill();
        split.fill();

        var relativeHealth = self.health;
        var indicatorColor = self.criticalColor;
        var portionColor   = self.badColor;

        if self.health > 2 {
            portionColor = self.goodColor;
            indicatorColor = self.mediumColor;

            relativeHealth -= 2.0;
        } else if self.health > 1 {
            portionColor = self.mediumColor;
            indicatorColor = self.badColor;

            relativeHealth -= 1.0;
        }

        CGContextRestoreGState(c);
        CGContextSaveGState(c);

        let rect = CGRectInset(bounds, bezelWidth, bezelWidth);
        let circleContent = UIBezierPath(ovalInRect: rect);
        circleContent.addClip();
        indicatorColor.setFill();
        circleContent.fill();

        let portionClip = UIBezierPath();
        portionClip.moveToPoint(viewCenter);
        let radius = midX-self.bezelWidth+0.5;

        CGContextRestoreGState(c);
        CGContextSaveGState(c);

        let endAngle = (270.001 - (Double(relativeHealth) * 360.0)) * M_PI / 180.0;
        portionClip.addArcWithCenter(viewCenter, radius: radius, startAngle:CGFloat(-90.0 * M_PI / 180.0), endAngle:CGFloat(endAngle), clockwise:false);
        portionClip.addClip();
        portionColor.setFill();
        portionClip.fill();

        CGContextRestoreGState(c);
    }
}
